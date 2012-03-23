<?php

/**
 * @file
 * The API and related functions for executing and managing payments.
 */

/**
 * Base class with common functionality.
 */
class PaymentCommon {
  function __construct(array $properties = array()) {
    foreach ($properties as $property => $value) {
      $this->$property = $value;
    }
  }
}

/**
 * A single payment. Contains all payment-specific data.
 *
 * @see PaymentMethod
 * @see PaymentMethodController
 */
class Payment extends PaymentCommon {

  /**
   * An array with PaymentAmountItem objects.
   *
   * @see Payment::__construct()
   * @see Payment::setAmount()
   *
   * @var array
   */
  public $amounts = array();

  /**
   * Information about this payment that is specific to the 'context' that
   * created it, such as the webshop.
   *
   * @var array
   */
  public $caller_data = array();

  /**
   * The Unix datetime the payment was created.
   *
   * @var integer
   */
  public $created = 0;

  /**
   * The ISO 4217 currency code of the amount.
   *
   * @var string
   */
  public $currency_code = 'XXX';

  /**
   * The Unix datetime the payment expires.
   *
   * @var integer
   */
  public $expired = 0;

  /**
   * The name of a function to call when payment execution is completed,
   * regardless of the payment status. It receives one argument:
   * - $payment Payment
   *   The Payment object.
   * The callback does not need to return anything and is free to redirect the
   * user or display something.
   * Use Payment::caller_data to pass on arbitrary data to the finish callback.
   *
   * @var string
   */
  public $finish_callback = '';

  /**
   * The payment method used for this payment.
   *
   * @var PaymentMethod
   */
  public $method = NULL;

  /**
   * Information about this payment that is specific to its payment method.
   *
   * @var array
   */
  public $method_data = array();

  /**
   * The internal ID of this payment.
   *
   * @var integer
   */
  public $pid = 0;

  /**
   * The status log. Contains PaymentStatusItem objects ordered by datetime. Do
   * not set directly, but use Payment::setStatus() instead.
   *
   * @see Payment::setStatus()
   *
   * @var array
   */
  public $statuses = array();

  /**
   * The UID of the user this payment belongs to.
   *
   * @var integer
   */
  public $uid = 0;

  /**
   * The Unix datetime the payment was last updated.
   *
   * @var integer
   */
  public $updated = 0;

  /**
   * Constructor.
   *
   * @param $properties array
   *   An associative array. Keys are property names and values are property
   *   values.
   *
   * @return NULL
   */
  function __construct(array $properties = array()) {
    global $user;

    parent::__construct($properties);

    if (!$this->uid) {
      $this->uid = $user->uid;
    }
    if (!$this->created) {
      $this->created = time();
    }
    if (!$this->statuses) {
      // We purposefully avoid Payment::setStatus(), because this is the
      // payment's first status.
      $this->statuses[] = new PaymentStatusItem(PAYMENT_STATUS_NEW);
    }
  }

  /**
   * Execute the actual payment.
   *
   * @return NULL
   */
  function execute() {
    // Preprocess the payment.
    module_invoke_all('payment_pre_execute', $this);
    if (module_exists('rules')) {
      rules_invoke_event('payment_pre_execute', $this);
    }
    // Execute the payment.
    if ($this->method && $this->method->validate($this)) {
      $this->setStatus(new PaymentStatusItem(PAYMENT_STATUS_PENDING));
      $this->method->controller->execute($this);
    }
    else {
      $this->setStatus(new PaymentStatusItem(PAYMENT_STATUS_FAILED));
    }
    // This is only called if the payment execution didn't redirect the user
    // offsite. Otherwise it's the payment method return page's responsibility.
    $this->finish(FALSE);
  }

  /**
   * Finish the payment after its execution.
   *
   * @return NULL
   */
  function finish() {
    entity_save('payment', $this);
    call_user_func($this->finish_callback, $this);
  }

  /**
   * Set a payment amount.
   *
   * @see payment_amount_extract()
   *
   * @param $name string
   *   The machine name of this amount.
   * @param $amount float
   *   The payment amount. The number of decimals depends on the ISO 4217
   *   currency used.
   * @param $description string
   *   A US English human-readable description for this amount. May contain HTML.
   * @param $description_arguments array
   *   Arguments to pass on to t() when translating $description.
   *
   * @return Payment
   */
  function setAmount($name, $amount, $description = '', $description_arguments = array()) {
    $this->amounts[$name] = new PaymentAmountItem(array(
      'amount' => $amount,
      'name' => $name,
      'description' => $description,
      'description_arguments' => $description_arguments,
    ));

    return $this;
  }

  /**
   * Get the total payment amount.
   *
   * @param $amounts array
   *   An array with PaymentAmountItem objects to calculate the total from.
   *   Leave empty to use $this->amounts.
   *
   * @return float
   */
  function totalAmount(array $amounts = array()) {
    $total = 0;
    if (!$amounts) {
      $amounts = $this->amounts;
    }
    foreach ($amounts as $amount) {
      $total += $amount->amount;
    }

    return $total;
  }

  /**
   * Set the payment status.
   *
   * @param $status_item PaymentStatusItem
   *
   * @return Payment
   */
  function setStatus(PaymentStatusItem $status_item) {
    // A fail-safe to ensure hook implementations are only called if the status
    // has really changed.
    if ($this->getStatus()->status == $status_item->status) {
      return;
    }

    $previous_status_item = $this->getStatus();
    $status_item->pid = $this->pid;
    $this->statuses[] = $status_item;
    foreach (module_implements('payment_status_change') as $module_name) {
      call_user_func($module_name . '_payment_status_change', $this, $previous_status_item);
      // If a hook invocation has added another log item, a new loop with
      // invocations has already been executed and we don't need to continue
      // with this one.
      if ($this->getStatus()->status != $status_item->status) {
        return;
      }
    }
    if (module_exists('rules')) {
      rules_invoke_event('payment_status_change', $this, $previous_status_item);
    }

    return $this;
  }

  /**
   * Get the current payment status.
   *
   * @return PaymentStatusItem
   */
  function getStatus() {
    return end($this->statuses);
  }

  /**
   * Get available/valid payment methods for this payment.
   *
   * @param $payment_methods array
   *   An array with the PaymentMethod objects to check for. Use an empty
   *   array to check the availability of all payment methods.
   *
   * @param array
   *   An array with PaymentMethod objects usable for Payment in its current
   *   state, keyed by PMID.
   */
  function availablePaymentMethods(array $payment_methods = array()) {
    if (!$payment_methods) {
      $payment_methods = entity_load('payment_method', FALSE);
    }
    $available = array();
    foreach ($payment_methods as $payment_method) {
      if ($payment_method->validate($this)) {
        $available[$payment_method->pmid] = $payment_method;
      }
    }

    return $available;
  }
}

/**
 * Entity API controller for payment entities.
 */
class PaymentEntityController extends EntityAPIController {

  /**
   * Implements EntityAPIController::load().
   */
  function load($ids = array(), $conditions = array()) {
    $entities = parent::load($ids, $conditions);
    foreach ($entities as $payment) {
      // Cast non-string scalars to their original types, because some backends
      // store/return all variables as strings.
      $payment->created = (int) $payment->created;
      $payment->expired = (int) $payment->expired;
      $payment->pid = (int) $payment->pid;
      $payment->uid = (int) $payment->uid;
      $payment->updated = (int) $payment->updated;
    }

    return $entities;
  }

  /**
   * Implements EntityAPIController::attachLoad().
   */
  function attachLoad(&$queried_entities, $revision_id = FALSE) {
    $pids = array_keys($queried_entities);

    // Load the payments's payment methods.
    $pmids = array();
    foreach ($queried_entities as $payment) {
      $pmids[] = $payment->pmid;
    }
    $methods = entity_load('payment_method', $pmids);

    // Load payment amounts.
    $result = db_select('payment_amount')
      ->fields('payment_amount')
      ->condition('pid', $pids)
      ->execute();
    $amounts = array();
    while ($amount_data = $result->fetchAssoc('name', PDO::FETCH_ASSOC)) {
      $amount_data['description_arguments'] = unserialize($amount_data['description_arguments']);
      $amount_data['amount'] = (float) $amount_data['amount'];
      $amounts[$amount_data['pid']][$amount_data['name']] = new PaymentAmountItem($amount_data);
    }

    // Load the payments's status items.
    $result = db_select('payment_status_item')
      ->fields('payment_status_item')
      ->condition('pid', $pids)
      ->orderBy('psiid')
      ->execute();
    $status_items = array();
    while ($data = $result->fetchObject()) {
      $status_item = new PaymentStatusItem($data->status, $data->created, $data->pid, $data->psiid);
      $status_items[$status_item->pid][] = $status_item;
    }

    // Add all data to the payments.
    foreach ($queried_entities as $payment) {
      $payment->statuses = $status_items[$payment->pid];
      $payment->amounts = $amounts[$payment->pid];
      $payment->method = $methods[$payment->pmid];
      unset($payment->pmid);
    }

    parent::attachLoad($queried_entities, $revision_id);
  }

  /**
   * Implements EntityAPIController::save().
   */
  function save($entity, DatabaseTransaction $transaction = NULL) {
    $payment = $entity;

    // Save the payment.
    $payment->updated = time();
    $payment->pmid = $payment->method->pmid;
    $return = parent::save($payment, $transaction);
    unset($payment->pmid);

    // Save payment amounts.
    foreach ($payment->amounts as $amount) {
      db_merge('payment_amount')
        ->key(array(
          'name' => $amount->name,
          'pid' => $payment->pid,
        ))
        ->fields(array(
          'amount' => $amount->amount,
          'description' => $amount->description,
          'description_arguments' => serialize($amount->description_arguments),
          'name' => $amount->name,
          'pid' => $payment->pid,
        ))
        ->execute();
    }

    // Save the payment's status items.
    foreach ($payment->statuses as $status_item) {
      // Statuses cannot be edited, so only save the ones without a PSIID set.
      if (!$status_item->psiid) {
        $status_item->pid = $payment->pid;
        drupal_write_record('payment_status_item', $status_item);
      }
    }

    return $return;
  }

  /**
   * Implements EntityAPIController::view().
   */
  function view($entities, $view_mode = 'full', $langcode = NULL, $page = NULL) {
    $build = parent::view($entities, $view_mode, $langcode, $page);
    foreach ($build['payment'] as &$payment_build) {
      $payment = $payment_build['#entity'];

      // Payment amounts.
      $header = array(t('Description'), t('Amount'));
      $rows = array();
      foreach ($payment->amounts as $name => $amount) {
        $rows[] = array(
          'data' => array(t($amount->description, $amount->description_arguments), payment_amount_human_readable($amount->amount, $payment->currency_code)),
          'class' => array('payment-amount-' . $name),
        );
      }
      $rows[] = array(
        'data' => array(t('Total amount'), payment_amount_human_readable($payment->totalAmount(), $payment->currency_code)),
        'class' => array('payment-amount-total'),
      );
      $payment_build['amounts'] = array(
        '#type' => 'markup',
        '#markup' => theme('table', array(
          'header' => $header,
          'rows' => $rows,
          'caption' => t('Payment amount'),
        )),
      );

      // Status history.
      $status = payment_status_info($payment->getStatus()->status, TRUE);
      $rows = array();
      foreach (array_reverse($payment->statuses) as $status_item) {
        $status = payment_status_info($status_item->status);
        $rows[] = array($status->title, format_date($status_item->created));
      }
      $header = array(t('Status'), t('Date'));
      $payment_build['history'] = array(
        '#type' => 'markup',
        '#markup' => theme('table', array(
          'header' => $header,
          'rows' => $rows,
          'caption' => t('Status history'),
        )),
      );
    }

    return $build;
  }

  /**
   * Implements EntityAPIController::delete().
   */
  function delete($ids, DatabaseTransaction $transaction = NULL) {
    parent::delete($ids, $transaction);
    db_delete('payment_amount')
      ->condition('pid', $ids)
      ->execute();
    db_delete('payment_status_item')
      ->condition('pid', $ids)
      ->execute();
  }
}

/**
 * Payment method configuration.
 *
 * @see Payment
 * @see PaymentMethodController
 */
class PaymentMethod extends PaymentCommon {

  /**
   * The payment method controller this merchant uses.
   *
   * @var PaymentMethodController
   */
  public $controller = NULL;

  /**
   * Information about this payment method that is specific to its controller.
   *
   * @var array
   */
  public $controller_data = array();

  /**
   * The entity's unique machine name.
   *
   * @var string
   */
  public $name = '';

  /**
   * The machine name of the module that created this payment method.
   *
   * @var string
   */
  public $module = 'payment';

  /**
   * The unique internal ID.
   *
   * @var integer
   */
  public $pmid = 0;

  /**
   * One of Entity API's ENTITY_* constants for exportable entities.
   *
   * @see entity_has_status()
   *
   * @var integer
   */
  public $status = '';

  /**
   * The specific human-readable title, e.g. "Paypal WPS".
   *
   * @var string
   */
  public $title_specific = '';

  /**
   * The generic human-readable title, e.g. "Paypal".
   *
   * @var string
   */
  public $title_generic = NULL;

  /**
   * Validate a payment against this payment method.
   *
   * @param $payment Payment
   *
   * @return boolean
   *   Whether the payment can work with the this payment method.
   */
  function validate(Payment $payment) {
    if ($this->controller->validate($payment, $this) !== TRUE) {
      return FALSE;
    }
    foreach(module_implements('payment_validate') as $module) {
      if (module_invoke($module, 'payment_validate', $payment, $this) !== TRUE) {
        return FALSE;
      }
    }
    // No validator returned FALSE, so the payment is valid.
    return TRUE;
  }
}

/**
 * Entity API controller for payment_method entities.
 */
class PaymentMethodEntityController extends EntityAPIControllerExportable {

  /**
   * Implements EntityAPIController::load().
   */
  function load($ids = array(), $conditions = array()) {
    $entities = parent::load($ids, $conditions);
    foreach ($entities as $payment_method) {
      // Cast non-string scalars to their original types, because some backends
      // store/return all variables as strings.
      $payment_method->pmid = (int) $payment_method->pmid;
    }

    return $entities;
  }

  /**
   * Implements EntityAPIController::attachLoad().
   */
  function attachLoad(&$queries_entities, $revision_id = FALSE) {
    foreach ($queries_entities as $entity) {
      $entity->controller = payment_method_controller_load($entity->controller_class_name);
      if (!$entity->controller) {
        $entity->controller = payment_method_controller_load('PaymentMethodControllerUnavailable');
      }
      unset($entity->controller_class_name);
    }
    parent::attachLoad($queries_entities, $revision_id);
  }

  /**
   * Implements EntityAPIController::save().
   */
  function save($entity, DatabaseTransaction $transaction = NULL) {
    $entity->controller_class_name = $entity->controller->name;
    $return = parent::save($entity, $transaction);
    unset($entity->controller_class_name);

    return $return;
  }

  /**
   * Implements EntityAPIController::export().
   */
  function export($entity, $prefix = '') {
    // The payment method controller should not be exported. Instead, we
    // temporarily replace it with a property that only stores its class name.
    $controller = $entity->controller;
    $entity->controller_class_name = $controller->name;
    unset($entity->controller);
    $export = parent::export($entity, $prefix);
    $entity->controller = $controller;
    unset($entity->controller_class_name);

    return $export;
  }

  /**
   * Implements EntityAPIController::import().
   */
  function import($export) {
    if ($payment = parent::import($export)) {
      $payment->controller = payment_method_controller_load($payment->controller_class_name);
      unset($payment->controller_class_name);
      return $payment;
    }
    return FALSE;
  }
}

/**
 * Features controller for payment_method entities.
 */
class PaymentMethodFeaturesController extends EntityDefaultFeaturesController {

  function export($data, &$export, $module_name = '') {
    $pipe = parent::export($data, $export, $module_name);

    // Add dependencies for the payment method controller classes.
    $controller_class_names = array();
    foreach (entity_load_multiple_by_name($this->type, $data) as $method) {
      $controller_class_names[] = $method->controller->name;
    }
    $result = db_select('registry')
      ->fields('registry', array('module'))
      ->condition('name', $controller_class_names)
      ->condition('type', 'class')
      ->execute();
    while ($module = $result->fetchField()) {
      $export['dependencies'][$module] = $module;
    }

    return $pipe;
  }
}

/**
 * A payment method controller, e.g. the logic behind a payment method.
 *
 * @see payment_method_controller_load()
 * @see payment_method_controller_load_multiple()
 *
 * All other payment methods need to extend this class. This is a singleton
 * class. See payment_method_controller_load().
 *
 * @see Payment
 * @see PaymentMethod
 */
class PaymentMethodController {

  /**
   * An array with ISO 4217 currency codes that this controller supports.
   *
   * Leave empty to allow all currencies.
   *
   * @var array
   */
  public $currencies = array();

  /**
   * A human-readable description of this payment method controller.
   *
   * @var string
   */
  public $description = '';

  /**
   * The machine name.
   *
   * This will be set by payment_method_controller_load_multiple() as a
   * shorthand for get_class($payment_method_controller).
   *
   * @see payment_method_controller_load_multiple()
   *
   * @var string
   */
  public $name = '';

  /**
   * The name of the function that returns form elements to configure this
   * payment method for a specific payment. Using #element_validate callbacks,
   * input can be added to the Payment object in $form_state['payment'].
   *
   * @var string
   */
  public $payment_form_elements_callback = '';

  /**
   * The form ID of the payment method settings form.
   *
   * The form's submit callback is expected to add all relevant information to
   * the PaymentMethod object in $form_sate['payment_method], so
   * payment_form_payment_method_submit() can call entity_save() on it.
   *
   * @var string
   */
  public $payment_method_form_id = 'payment_form_payment_method_controller';

  /**
   * The human-readable title.
   *
   * @var array
   */
  public $title = '';

  /**
   * Execute a payment.
   *
   * @param $payment Payment
   *
   * @return boolean
   *   Whether the payment was successfully executed or not.
   */
  function execute(Payment $payment) {}

  /**
   * Validate a payment against a payment method and this controller. Don't
   * call directly. Use PaymentMethod::validate() instead.
   *
   * @see PaymentMethod::validate()
   *
   * @param $payment Payment
   * @param $payment_method PaymentMethod
   *
   * @return boolean
   *   Whether the payment, the provided payment method and its controller can
   *   work together.
   */
  function validate(Payment $payment, PaymentMethod $payment_method) {
    $valid = TRUE;

    // Confirm the payment amount is higher than 0.
    if ($payment->amount <= 0) {
      $valid = FALSE;
    }

    // Confirm the payment's currency is supported.
    if (!empty($this->currencies) && !in_array($payment->currency_code, $this->currencies)) {
      $valid = FALSE;
    }

    return $valid;
  }
}

/**
 * A payment method controller that essentially disables payment methods.
 *
 * This is a 'placeholder' controller that returns defaults and doesn't really
 * do anything else. It is used when no working controller is available for a
 * payment method, so other modules don't have to check for that.
 */
class PaymentMethodControllerUnavailable extends PaymentMethodController {

  function __construct() {
    $this->title = t('Unavailable');
  }

  /**
   * Implements PaymentMethodController::execute().
   */
  function execute(Payment $payment) {
    $payment->setStatus(new PaymentStatusItem(PAYMENT_STATUS_UNKNOWN));
  }

  /**
   * Implements PaymentMethodController::validate().
   */
  function validate(Payment $payment, PaymentMethod $payment_method) {
    return FALSE;
  }
}

/**
 * Form build callback: implements PaymentMethodController payment method form.
 *
 * @return array
 *   A Drupal form.
 */
function payment_form_payment_method_controller(array $form, array &$form_state) {
  return $form;
}

/**
 * Payment status information.
 */
class PaymentStatusInfo extends PaymentCommon {

  /**
   * A US English human-readable description.
   *
   * @var string
   */
  public $description = '';

  /**
   * The machine names of this status's parent statuses.
   *
   * @var array
   */
  public $parents = array();

  /**
   * The status itself.
   *
   * @var string
   */
  public $status = '';

  /**
   * A US English human-readable title.
   *
   * @var string
   */
  public $title = '';

  /**
   * Get this payment status's ancestors.
   *
   * @return array
   *   The machine names of this status's ancestors.
   */
  function ancestors() {
    $ancestors = $this->parents;
    foreach ($this->parents as $status) {
      $parent = payment_status_info($status);
      $ancestors += $parent->ancestors();
    }

    return array_unique($ancestors);
  }
}

/**
 * A payment amount line item.
 *
 * @see Payment::setAmount()
 */
class PaymentAmountItem extends PaymentCommon {

  /**
   * The payment amount. The number of decimals depends on the ISO 4217
   * currency used.
   *
   * @var float
   */
  public $amount = 0;

  /**
   * A US English human-readable description for this amount. May contain HTML.
   *
   * @var string
   */
  public $description = '';

  /**
   * Arguments to pass on to t() when translating $this->description.
   *
   * @var array
   */
  public $description_arguments = array();

  /**
   * The machine name of this amount.
   *
   * @var string
   */
  public $name = '';
}
/**
 * A payment status line item.
 */
class PaymentStatusItem {

  /**
   * The status itself.
   *
   * @var string
   */
  public $status = '';

  /**
   * The Unix datetime this status was set.
   *
   * @var integer
   */
  public $created = 0;

  /**
   * The PID of the payment this status item belongs to.
   *
   * @var integer
   */
  public $pid = 0;

  /**
   * The unique internal ID of this payment status item.
   *
   * @var integer
   */
  public $psiid = 0;

  function __construct($status, $created = 0, $pid = 0, $psiid = 0) {
    $this->status = $status;
    $this->created = $created ? $created : time();
    $this->pid = $pid;
    $this->psiid = $psiid;
  }
}

/**
 * Information about a type of payment amount.
 */
class PaymentAmountInfo extends PaymentCommon {

  /**
   * The callback function to extract this payment amount from the Payment.
   *
   * The function accepts the machine name of the amount to extract and the
   * Payment object to extract the amount from as parameters. It should return
   * an array with PaymentAmountItem objects or FALSE if the Payment object
   * does not contain any matching amount items.
   *
   * @see payment_amount_extract()
   * @see payment_amount_extract_total()
   *
   * @var string
   */
  public $callback = '';

  /**
   * The payment amounts's machine name.
   *
   * @var string
   */
  public $name = '';

  /**
   * The payment amounts's human-readable title.
   *
   * @var string
   */
  public $title = '';
}