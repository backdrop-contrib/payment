<?php

/**
 * @file
 * The API and related functions for executing and managing payments.
 */

/**
 * Base class with common functionality.
 */
class PaymentCommon {
  function __construct(array $properties) {
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
   * The payment amount.
   *
   * The number of decimals depends on the ISO 4217 currency used.
   *
   * @var float
   */
  public $amount = 0;

  /**
   * The name of a function to call when the payment has been completed. It
   * receives two arguments:
   * - $payment Payment
   *   The Payment object.
   * - $redirect boolean
   *   Whether the user was redirected offsite during payment execution.
   * The callback does not need to return anything and is free to redirect the
   * user or display something.
   * Use $caller_data to pass on arbitrary data to the finish callback.
   *
   * @var string
   */
  public $finish_callback = '';

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
   * The payment description.
   *
   * @var string
   */
  public $description = '';

  /**
   * The Unix datetime the payment expires.
   *
   * @var integer
   */
  public $expired = 0;

  /**
   * Information about this payment that is specific to the 'context' that
   * created it, such as the webshop.
   *
   * @var array
   */
  public $caller_data = array();

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
   * The current status. Do not set directly.
   *
   * @see Payment::setStatus()
   *
   * @var string
   */
  public $status = PAYMENT_STATUS_NEW;

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
   * The internal revision ID of this payment.
   *
   * @var integer
   */
  public $vid = 0;

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

    $this->uid = $user->uid;
    $this->created = time();
    parent::__construct($properties);
  }

  /**
   * Execute the actual payment.
   *
   * @return NULL
   */
  function execute() {
    if ($this->method && $this->method->controller->validate($this)) {
      $this->setStatus(PAYMENT_STATUS_PENDING);
      $this->method->controller->execute($this);
    }
    else {
      $this->setStatus(PAYMENT_STATUS_FAILED);
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
   * Set the payment status.
   *
   * @param $status
   *
   * @return NULL
   */
  function setStatus($status) {
    // A fail-safe to ensure hook implementations are only called if the status
    // has really changed.
    if ($this->status == $status) {
      return;
    }

    $old_status = $this->status;
    $this->status = $status;
    foreach (module_implements('payment_status_change') as $module_name) {
      call_user_func($module_name . '_payment_status_change', $this, $old_status);
      // If a hook invocation has changed the status, a new loop with
      // invocations has already been executed and we don't need to continue
      // with this one.
      if ($this->status != $status) {
        return;
      }
    }
    if (module_exists('rules')) {
      rules_invoke_event('payment_status_change', $this, $old_status);
    }
  }
}

/**
 * Entity API controller for payment entities.
 */
class PaymentEntityController extends EntityAPIController {

  /**
   * Implements EntityAPIController::attachLoad().
   */
  function attachLoad(&$queries_entities, $revision_id = FALSE) {
    foreach ($queries_entities as $entity) {
      $entity->method = entity_load_single('payment_method', $entity->pmid);
      unset($entity->pmid);
    }
    parent::attachLoad($queries_entities, $revision_id);
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
    // Insert a new revision.
    $payment->created_revision = time();
    $payment->vid = 0;
    drupal_write_record('payment_revision', $payment);
    // Update the payment with the new revision information.
    db_update('payment')
      ->fields(array(
        'vid' => $payment->vid,
      ))
      ->condition('pid', $payment->pid)
      ->execute();

    return $return;
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
   * The unique internal ID.
   *
   * @var integer
   */
  public $pmid = 0;

  /**
   * Whether or not to use the controllers testing environment if there is any.
   *
   * @var boolean
   */
  public $test = FALSE;

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
}

/**
 * Entity API controller for payment_method entities.
 */
class PaymentMethodEntityController extends EntityAPIController {

  /**
   * Implements EntityAPIController::attachLoad().
   */
  function attachLoad(&$queries_entities, $revision_id = FALSE) {
    foreach ($queries_entities as $entity) {
      $entity->controller = payment_method_controller_load($entity->controller_class_name);
      unset($entity->controller_class_name);
    }
    parent::attachLoad($queries_entities, $revision_id);
  }

  /**
   * Implements EntityAPIController::save().
   */
  function save($entity, DatabaseTransaction $transaction = NULL) {
    $entity->controller_class_name = get_class($entity->controller);
    $return = parent::save($entity, $transaction);
    unset($entity->controller_class_name);

    return $return;
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
   * paymentui_form_payment_method_submit() can call entity_save() on it.
   *
   * @var string
   */
  public $payment_method_form_id = 'payment_form_payment_method';

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
   * Validate a payment's data.
   *
   * Because we're dealing with real money, this is a 'last defense' in case
   * payment data is missing or incorrect.
   *
   * @param $payment Payment
   *
   * @return boolean
   *  Whether the payment contains all necessary data for execution.
   */
  function validate(Payment $payment) {
    // @todo Add validation for minimum/maximum amount.
    // Confirm the payment's currency is supported.
    return empty($this->currencies) || in_array($payment->currency_code, $this->currencies);
  }
}

/**
 * A payment method controller that essentially disables payment methods.
 *
 * This is a 'placeholder' controller that returns defaults and doesn't really
 * do anything else. It is used when no working controller is available for a
 * payment method, so other modules don't have to check for that.
 */
class PaymentMethodControllerDisabled extends PaymentMethodController {

  function __construct() {
    $this->title = t('Disabled');
  }

  /**
   * Implements PaymentMethodController::execute().
   */
  function execute(Payment $payment) {
    $payment->setStatus(PAYMENT_STATUS_UNKNOWN);
  }

  /**
   * Implements PaymentMethodController::validate().
   */
  function validate(Payment $payment) {
    return FALSE;
  }
}

/**
 * Form build callback: implements PaymentMethodController payment method form.
 *
 * @return array
 *   A Drupal form.
 */
function payment_form_payment_method(array $form, array &$form_state) {
  return $form;
}