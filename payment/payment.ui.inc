<?php

/**
 * @file
 * The Payment user interface.
 */

/**
 * Menu title callback: return a payment's title.
 *
 * @param Payment $payment
 *
 * @return string
 */
function payment_title(Payment $payment) {
  return t('Payment !pid', array(
    '!pid' => $payment->pid,
  ));
}

/**
 * Menu page callback: show a message that describes the dependencies for
 * viewing this page have not been met.
 *
 * @param array $modules
 *   Keys are modules' machine names or numeric, values are modules'
 *   human-readable titles.
 *
 * @return string
 */
function payment_page_required_modules(array $modules) {
  foreach ($modules as $name => &$title) {
    if (is_string($name)) {
      $title = l($title, 'http://drupal.org/project/' . $name);
    }
  }

  return t('This page requires !modules.', array(
    '!modules' => implode(', ', $modules),
  ));
}

/**
 * Menu page callback: show a payment.
 *
 * @param Payment $payment
 *
 * @return array
 */
function payment_page_payment_view(Payment $payment) {
  return entity_view('payment', array($payment));
}

/**
 * Implements form build callback: the payment add/edit form.
 *
 * @param array $form
 * @param array $form_state
 * @param Payment $payment
 *
 * @return array
 *   A render array.
 */
function payment_form(array $form, array &$form_state, Payment $payment) {
  $form_state['payment'] = $payment;
  $form['status'] = array(
    '#access' => !empty($payment->pid),
    '#type' => 'select',
    '#title' => t('Status'),
    '#options' => payment_status_options(),
    '#default_value' => $payment->getStatus()->status,
    '#required' => TRUE,
    '#description' => t('Updating a payment status manually can disrupt automatic payment processing.') . (user_access('payment.status.view') ? ' ' . l(t('Payment status overview.'), 'admin/config/services/payment/status') : ''),
  );
  $form['line_items'] = payment_line_items($payment);
  $form['payment_method'] = array(
    '#access' => empty($payment->pid),
    '#type' => 'payment_method',
    '#title' => t('Payment method'),
    '#required' => TRUE,
  );
  $form['actions'] = array(
    '#type' => 'actions',
  );
  $form['actions']['save'] = array(
    '#type' => 'submit',
    '#value' => $payment->pid ? t('Save') : t('Pay'),
  );
  if ($payment->pid) {
    $form['actions']['delete'] = array(
      '#type' => 'link',
      '#title' => t('Delete'),
      '#href' => 'payment/' . $payment->pid . '/delete',
      '#access' => payment_access('delete', $payment),
    );
  }
  field_attach_form('payment', $payment, $form, $form_state);

  return $form;
}

/**
 * Implements form validate callback for payment_form().
 */
function payment_form_validate(array $form, array &$form_state) {
  $payment = $form_state['payment'];
  field_attach_form_validate('payment', $payment, $form, $form_state);

  if (empty($form_state['rebuild']) && $payment->method) {
    try {
      $payment->method->validate($payment);
    }
    catch (PaymentValidationException $e) {
      form_set_error('payment_method', $e->getMessage());
    }
  }
}

/**
 * Implements form submit callback for payment_form().
 */
function payment_form_submit(array $form, array &$form_state) {
  $payment = $form_state['payment'];
  $new = empty($payment->pid);

  // Process submitted values.
  $payment->setStatus(new PaymentStatusItem($form_state['values']['status']));
  field_attach_submit('payment', $payment, $form, $form_state);

  entity_save('payment', $payment);
  if (payment_access('view', $payment)) {
    $form_state['redirect'] = 'payment/' . $payment->pid;
  }

  if ($new) {
    $payment->execute();
    if (payment_status_has_ancestor($payment->getStatus()->status, PAYMENT_STATUS_FAILED)) {
      $form_state['rebuild'] = TRUE;
    }
  }
}

/**
 * Implements form build callback: payment deletion form.
 */
function payment_form_payment_delete(array $form, array &$form_state, Payment $payment) {
  $form_state['payment'] = $payment;

  return confirm_form($form, t('Do you really want to delete payment !pid?', array(
    '!pid' => $payment->pid,
  )), 'payment/' . $payment->pid, t('Existing information that uses this payment, such as a webshop order, may become unusable. This action cannot be undone.'), t('Delete payment'));
}

/**
 * Implements form submit callback for payment_form_payment_delete().
 */
function payment_form_payment_delete_submit(array $form, array &$form_state) {
  $payment = $form_state['payment'];
  entity_delete('payment', $payment->pid);
  $form_state['redirect'] = '<front>';
  drupal_set_message(t('Payment !pid has been deleted.', array(
    '!pid' => $payment->pid,
  )));
}

/**
 * Shows a page with controllers payment methods can be added for.
 *
 * @return string
 */
function payment_page_payment_method_add_select_controller() {
  $controllers = payment_method_controller_load_multiple();
  unset($controllers['PaymentMethodControllerUnavailable']);
  if ($controllers) {
    $items = array();
    foreach ($controllers as $controller) {
      $payment_method = new PaymentMethod(array(
        'controller' => $controller,
      ));
      if (payment_method_access('create', $payment_method)) {
        $items[] = array(
          'title' => $controller->title,
          'href' => 'admin/config/services/payment/method/add/' . $controller->name,
          'description' => $controller->description,
          'localized_options' => array(),
        );
      }
    }
    return theme('admin_block_content', array(
      'content' => $items,
    ));
  }
  else {
    return t('There are no payment method types available. Enable modules that provide them in order to add payment methods.');
  }
}

/**
 * Menu access callback for
 * payment_page_payment_method_add_select_controller().
 *
 * @return boolean
 *   TRUE if there is at least one payment method controller the current user
 *   has permission to create payment methods with.
 */
function payment_page_payment_method_add_select_controller_access() {
  $controllers = payment_method_controller_load_multiple();
  unset($controllers['PaymentMethodControllerUnavailable']);
  foreach ($controllers as $controller) {
    $payment_method = new PaymentMethod(array(
      'controller' => $controller,
    ));
    if (payment_method_access('create', $payment_method)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Menu access callback for payment_method_form_add().
 *
 * @param string $controller_class_name
 *   The name of the controller class the current user wants to add a payment
 *   method with.
 *
 * @return boolean
 */
function payment_method_form_add_access(PaymentMethodController $controller) {
  $payment_method = new PaymentMethod(array(
    'controller' => $controller,
  ));

  return payment_method_access('create', $payment_method);
}

/**
 * Create a blank payment method and return its payment form.
 *
 * @param string $controller_class_name
 *   The name of the controller class for which to create a payment method.
 *
 * @return array
 *   A Drupal form.
 */
function payment_method_form_add($controller) {
  $payment_method = new PaymentMethod(array(
    'controller' => $controller,
  ));

  return drupal_get_form('payment_form_payment_method', $payment_method);
}

/**
 * Implements form build callback: the payment method add/edit form.
 *
 * @see payment_forms()
 *
 * @param PaymentMethod $payment_method
 */
function payment_form_payment_method(array $form, array &$form_state, PaymentMethod $payment_method) {
  global $user;

  $form_state['payment_method'] = $payment_method;

  $form['controller'] = array(
    '#type' => 'item',
    '#title' => t('Type'),
    '#markup' => check_plain($payment_method->controller->title),
  );
  $form['enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enabled'),
    '#default_value' => $payment_method->enabled,
  );
  $form['title_specific'] = array(
    '#type' => 'textfield',
    '#title' => t('Title (specific)'),
    '#description' => t('The specific title is often only displayed to people such as administrators who need to know the exact payment method that is used, for instance <em>Paypal Website Payments Pro</em>.'),
    '#default_value' => $payment_method->title_specific,
    '#maxlength' => 255,
    '#required' => TRUE,
  );
  $form['name'] = array(
    '#type' => 'machine_name',
    '#default_value' => $payment_method->name,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#machine_name' => array(
      'source' => array('title_specific'),
      'exists' => 'payment_method_name_exists',
    ),
    '#disabled' => !empty($payment_method->pmid),
  );
  $form['title_generic'] = array(
    '#type' => 'textfield',
    '#title' => t('Title (generic)'),
    '#description' => t('The generic title is often only displayed to people such as payers who only need to know the generic payment method that is used, for instance <em>Paypal</em>. Defaults to the specific title.'),
    '#default_value' => $payment_method->title_generic,
    '#maxlength' => 255,
  );
  $form['owner'] = array(
    '#type' => 'textfield',
    '#title' => t('Owner'),
    '#default_value' => $payment_method->uid ? user_load($payment_method->uid)->name : $user->name,
    '#maxlength' => 255,
    '#autocomplete_path' => 'user/autocomplete', 
    '#required' => TRUE,
  );
  $form['controller_form'] = payment_method_controller_form_callback($form, $form_state, $payment_method->controller, 'payment_method', 'build');
  $form['actions'] = array(
    '#type' => 'actions',
  );
  $form['actions']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  if ($payment_method->pmid) {
    $form['actions']['delete'] = array(
      '#type' => 'link',
      '#title' => t('Delete'),
      '#href' => 'admin/config/services/payment/method/' . $payment_method->pmid . '/delete',
      '#access' => payment_method_access('delete', $payment_method),
    );
  }

  return $form;
}

/**
 * Implements form validate callback for payment_form_payment_method().
 */
function payment_form_payment_method_validate(array $form, array &$form_state) {
  $values = $form_state['values'];
  if (!($account = user_load_by_name($values['owner']))) {
    form_set_error('owner', t('The username %name does not exist.', array(
      '%name' => $values['owner'],
    )));
  }
  payment_method_controller_form_callback($form, $form_state, $form_state['payment_method']->controller, 'payment_method', 'validate');
}

/**
 * Implements form submit callback for payment_form_payment_method().
 */
function payment_form_payment_method_submit(array $form, array &$form_state) {
  $values = $form_state['values'];
  $payment_method = $form_state['payment_method'];
  $payment_method->enabled = $values['enabled'];
  $payment_method->name = $values['name'];
  $payment_method->title_specific = $values['title_specific'];
  // The generic title defaults to the specific one.
  $payment_method->title_generic = $values['title_generic'] ? $values['title_generic'] : $values['title_specific'];
  $payment_method->uid = user_load_by_name($values['owner'])->uid;
  entity_save('payment_method', $payment_method);
  $form_state['redirect'] = 'admin/config/services/payment/method';
  drupal_set_message(t('Payment method %title has been saved.', array(
    '%title' => $payment_method->title_specific,
  )));
}

/**
 * Implements form build callback: payment method deletion form.
 */
function payment_form_payment_method_delete(array $form, array &$form_state, PaymentMethod $payment_method) {
  $form_state['payment_method'] = $payment_method;

  return confirm_form($form, t('Do you really want to delete payment method %title?', array(
    '%title' => $form_state['payment_method']->title_specific,
  )), 'admin/config/services/payment/method', t('Existing payments that use this payment method will become unusable. This action cannot be undone.'), t('Delete payment method'));
}

/**
 * Implements form submit callback for payment_form_payment_method_delete().
 */
function payment_form_payment_method_delete_submit(array $form, array &$form_state) {
  $payment_method = $form_state['payment_method'];
  entity_delete('payment_method', $payment_method->pmid);
  $form_state['redirect'] = 'admin/config/services/payment/method';
  drupal_set_message(t('Payment method %title has been deleted.', array(
    '%title' => $payment_method->title_specific,
  )));
}

/**
 * Display a payment status overview.
 *
 * @return string
 */
function payment_page_status() {
  return theme('table', array(
    'header' => array(t('Title'), t('Description')),
    'rows' => _payment_page_status_level(payment_status_hierarchy(), 0),
  ));
}

/**
 * Helper function for payment_page_status() to build table rows.
 *
 * @param array $hierarchy
 *   A payment status hierarchy as returned by payment_status_hierarchy().
 * @param integer $depth
 *   The depth of $hierarchy's top-level items as seen from the original
 *   hierarchy's root (this function is recursive), starting with 0.
 *
 * @return array
 */
function _payment_page_status_level(array $hierarchy, $depth) {
  $rows = array();
  foreach ($hierarchy as $status => $children) {
    $status_info = payment_status_info($status);
    $rows[] = array(theme('indentation', array(
    'size' => $depth,
  )) . $status_info->title, $status_info->description);
    $rows = array_merge($rows, _payment_page_status_level($children, $depth + 1));
  }

  return $rows;
}

/**
 * Helper function to build the rows for the table in payment_page_status().
 *
 * @see payment_page_status()
 *
 * @param array $statuses
 *   The statuses for which to build table rows.
 * @param array $children
 *   Keys are payment statuses, values are the statuses that are the keys's
 *   children.
 * @param array $rows
 *   The table rows to which to add new ones.
 * @param integer $depth
 */
function _payment_page_status_rows(array $statuses, array $children, array &$rows, $depth = 0) {
  foreach ($statuses as $status) {
    $rows[] = array(theme('payment_page_status_row', array(
      'status' => $status,
      'depth' => $depth,
    )));
    if (isset($children[$status])) {
      _payment_page_status_rows($children[$status], $children, $rows, $depth + 1);
    }
  }
}

/**
 * Implements uasort() callback to sort PaymentStatusInfo objects by title.
 */
function payment_payment_status_sort_title(PaymentStatusInfo $status_a, PaymentStatusInfo $status_b) {
  return strcmp($status_a->title, $status_b->title);
}

function payment_method_name_exists($name) {
  return !is_a(entity_load_single('payment_method', $name), 'PaymentMethodUnavailable');
  
}

/**
 * Return a render array containing a Payment's line items.
 *
 * @param Payment $payment
 *
 * @return array
 */
function payment_line_items(Payment $payment) {
  $rows = array();
  foreach ($payment->line_items as $name => $line_item) {
    $rows[] = array(
      'data' => array(
        t($line_item->description, $line_item->description_arguments),
        $line_item->quantity,
        payment_amount_human_readable($line_item->unitAmount(TRUE), $payment->currency_code),
        payment_amount_human_readable($line_item->totalAmount(TRUE), $payment->currency_code),
        t('!amount (!percentage%)', array(
          '!amount' => payment_amount_human_readable($line_item->amount * $line_item->tax_rate, $payment->currency_code),
          '!percentage' => $line_item->tax_rate * 100,
        )),
      ),
      'class' => array('payment-line_item-' . $name),
    );
  }
  $rows[] = array(
    'data' => array(array(
      'data' => t('Total amount'),
      'colspan' => 3,
      ), payment_amount_human_readable($payment->totalAmount(TRUE), $payment->currency_code), '',
    ),
    'class' => array('payment-line_item-total'),
  );
  $build = array(
    '#type' => 'markup',
    '#markup' => theme('table', array(
      'header' => array(t('Description'), t('Quantity'), t('Amount'), t('Total'), t('Tax')),
      'rows' => $rows,
    )),
  );

  return $build;
}

/**
 * Return a render array containing a Payment's status items.
 *
 * @param Payment $payment
 *
 * @return array
 */
function payment_status_items(Payment $payment) {
  $status = payment_status_info($payment->getStatus()->status, TRUE);
  $rows = array();
  foreach (array_reverse($payment->statuses) as $status_item) {
    $status = payment_status_info($status_item->status);
    $rows[] = array($status->title, format_date($status_item->created));
  }
  $build['status_items'] = array(
    '#type' => 'markup',
    '#markup' => theme('table', array(
      'header' => array(t('Status'), t('Date')),
      'rows' => $rows,
    )),
  );

  return $build;
}

/**
 * Toggle a payment method's enabled status.
 *
 * @param string $operation
 *   Either 'enable' or 'disable'.
 * @param PaymentMethod $payment_method
 *
 * @return NULL
 */
function payment_page_method_enable_disable($operation, PaymentMethod $payment_method) {
  switch ($operation) {
    case 'enable':
      $payment_method->enabled = TRUE;
      break;
    case 'disable':
      $payment_method->enabled = FALSE;
      break;
  }
  entity_save('payment_method', $payment_method);
  drupal_goto('admin/config/services/payment/method');
}

/**
 * Clone a payment method and return its payment form.
 *
 * @param PaymentMethod $payment_method
 *   The payment method to clone.
 *
 * @return array.
 */
function payment_page_method_clone(PaymentMethod $payment_method) {
  $payment_method = clone $payment_method;
  $payment_method->pmid = 0;
  $payment_method->name = '';
  drupal_set_message(t('You are now editing an unsaved clone of payment method %title.', array(
    '%title' => $payment_method->title_specific,
  )));

  return drupal_get_form('payment_form_payment_method', $payment_method);
}

/**
 * Implements form process callback for a payment_amount element.
 */
function payment_form_process_amount(array $element) {
  $element['#type'] = 'textfield';
  $element['#field_prefix'] = $element['#currency_code'];
  $description = NULL;
  if ($element['#minimum_amount'] !== FALSE && $element['#minimum_amount'] > 0) {
    $description = t('The minimum amount is !amount.', array(
      '!amount' => payment_amount_human_readable($element['#minimum_amount'], $element['#currency_code']),
    ));
  }
  $element['#description'] = $description;
  $element['#size'] = 16;
  $element['#maxlength'] = 16;
  $element += element_info('textfield');

  return $element;
}

/**
 * Implements form validate callback for a payment_amount element.
 */
function payment_form_process_amount_validate(array $element, array &$form_state) {
  $value = $element['#value'];

  // Do nothing if there is no value and the element is optional.
  if (!$element['#required'] && $value === '') {
    return;
  }

  // Quickly check for invalid characters.
  if (preg_match('#[^-\d.,]#', $value)) {
    form_error($element, t('The amount can only consist of a minus sign, decimals and one decimal mark.'));
  }
  // Do a final extensive check on the allowed format and allowed characters.
  elseif (!preg_match('/^
    -?    # One optional minus sign.
    \d+?  # One or more digits.
    [.,]? # One period or comma as optional decimal separator.
    \d*   # Zero or more decimals.
    $/x', $value)) {
    form_error($element, t('The amount can only consist of digits, optionally preceded by a minus sign and optionally preceded, separated or succeeded by a decimal separator.'));
  }
  else {
    // Convert the value to a float.
    $amount = (float) $value;

    // Confirm the amount lies within the allowed range.
    if ($element['#minimum_amount'] !== FALSE && $amount < $element['#minimum_amount']) {
      form_error($element, t('The minimum amount is !amount.', array(
        '!amount' => payment_amount_human_readable($element['#minimum_amount'], $element['#currency_code']),
      )));
    }

    // The value passed validation. Set the amount as a float as the value for
    // further processing.
    else {
      form_set_value($element, $amount, $form_state);
    }
  }
}

/**
 * Implements form process callback for a payment_line_item element.
 */
function payment_form_process_line_item(array $element, array &$form_state, array $form) {
  $form_state[drupal_clean_css_identifier('payment_' . $element['#name'])] = $element['#parents'];

  // Fetch all line items to display elements for.
  $line_items = array();
  // We're building the form for the first time.
  if (!isset($form_state['payment_line_item_count'])) {
    // Track one empty line item by default.
    $form_state['payment_line_item_count'] = count($line_items) + 1;
    // Track and add the default values.
    if (isset($element['#default_value'])) {
      $line_items = array_values($element['#default_value']);
      $form_state['payment_line_item_count'] += count($element['#default_value']);
    }
  }
  // Add any tracked line items that haven't been added yet.
  if ($form_state['payment_line_item_count'] > count($line_items)) {
    $diff = $form_state['payment_line_item_count'] - count($line_items);
    $line_items = array_merge($line_items, array_fill(0, $diff, NULL));
  }

  // Build the line items.
  foreach ($line_items as $delta => $line_item) {
    $required = $delta == 0 && $element['#required'];
    $element['container_' . $delta] = array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('payment-line-item-container payment-line-item-container-' . $delta, ($delta + 1) % 2 == 0 ? 'odd' : 'even'),
      ),
    );
    $element['container_' . $delta]['amount'] = array(
      '#type' => 'payment_amount',
      '#title' => t('Amount'),
      '#default_value' => $line_item ? $line_item->amount : '',
      '#currency_code' => $element['#currency_code'],
      '#required' => $required,
      '#attributes' => array(
        'class' => array('payment-line-item-amount'),
      ),
    );
    $element['container_' . $delta]['quantity'] = array(
      '#type' => 'textfield',
      '#title' => t('Quantity'),
      '#default_value' => $line_item ? $line_item->quantity : '',
      '#size' => 3,
      '#required' => $required,
    );
    $element['container_' . $delta]['tax_rate'] = array(
      '#type' => 'textfield',
      '#title' => t('Tax rate'),
      '#default_value' => $line_item ? $line_item->tax_rate * 100 : '',
      '#size' => 5,
      '#field_suffix' => '%',
      '#required' => $required,
    );
    $element['container_' . $delta]['description'] = array(
      '#type' => 'textfield',
      '#title' => t('Description'),
      '#default_value' => $line_item ? $line_item->description : '',
      '#required' => $required,
      '#maxlength' => 255,
    );
    $element['container_' . $delta]['name'] = array(
      '#type' => 'machine_name',
      '#default_value' => $line_item ? $line_item->name : '',
      '#maxlength' => 255,
      '#required' => TRUE,
      '#machine_name' => array(
        'source' => array_merge($element['#parents'], array('container_' . $delta, 'description')),
        'exists' => 'payment_method_name_exists',
      ),
      '#required' => $required,
    );
    $element['container_' . $delta]['clear'] = array(
      '#type' => 'markup',
      '#markup' => '<div class="clear"></div>',
    );
  }
  // "Add more line items" button.
  $wrapper_id = drupal_html_id('payment-ajax-replace');
  $element['add_more'] = array(
    '#type' => 'submit',
    '#value' => t('Add a line item'),
    '#submit' => array('payment_form_process_line_item_submit'),
    '#limit_validation_errors' => array(),
    '#ajax' => array(
      'callback' => 'payment_form_process_line_item_submit_ajax_callback',
      'effect' => 'fade',
      'event' => 'mousedown',
      'wrapper' => $wrapper_id,
    ),
    '#name' => drupal_clean_css_identifier('payment_' . $element['#name']),
    '#access' => $element['#cardinality'] == 0 || $form_state['payment_line_item_count'] < $element['#cardinality'],
    '#id' => $wrapper_id,
    '#attributes' => array(
      'class' => array('payment-add-more'),
    ),
  );

  return $element;
}

/**
 * Implements form validate callback for a payment_line_item element.
 */
function payment_form_process_line_item_validate(array $element, array &$form_state) {
  $values = drupal_array_get_nested_value($form_state['values'], $element['#parents']);
  // Don't let the submit button's value be validated.
  unset($values['add_more']);
  $line_items = array();
  foreach ($values as $container => $line_item_data) {
    // All this line item's elements are empty, so there's nothing to validate.
    if (reset($line_item_data) == '' && count(array_unique($line_item_data)) == 1) {
      break;
    }
    // They're not all empty, so make sure they all contain input.
    else {
      // Keep track
      $errors = array_fill_keys(array_keys($element['#value']), FALSE);
      foreach ($line_item_data as $property => $value) {
        if (!strlen($value)) {
          form_error($element[$container][$property], t('%title is required, or leave all fields for this line item empty.', array(
            '%title' => $element[$container][$property]['#title'],
          )));
        }
      }
    }

    // Validate quantity.
    if (preg_match('#\D#', $line_item_data['quantity'])) {
      form_error($element[$container]['quantity'], t('Quantity should be a positive integer.'));
    }

    // Validate tax rate.
    if (preg_match('#\D#', $line_item_data['tax_rate'])) {
      form_error($element, t('Tax rate should be a positive integer.'));
    }
    else {
      $line_item_data['tax_rate'] /= 100;
    }

    // Convert the raw input to a PaymentLineItem object.
    $line_item_data['amount'] = (float) $line_item_data['amount'];
    $line_item_data['quantity'] = (int) $line_item_data['quantity'];
    $line_item_data['tax_rate'] = (float) $line_item_data['tax_rate'];
    $line_items[] = new PaymentLineItem($line_item_data);
  }
  form_set_value($element, $line_items, $form_state);
}

/**
 * Implements form submit callback for payment_line_item elements.
 */
function payment_form_process_line_item_submit(array $form, array &$form_state) {
  $form_state['payment_line_item_count']++;
  $form_state['rebuild'] = TRUE;
}

/**
 * Implements form AJAX callback for payment_line_item elements.
 */
function payment_form_process_line_item_submit_ajax_callback(array $form, array &$form_state) {
  $element = drupal_array_get_nested_value($form, $form_state[$form_state['triggering_element']['#name']]);
  $container_key = 'container_' . ($form_state['payment_line_item_count'] - 1);

  return array(
    $container_key => $element[$container_key],
    'add_more' => $element['add_more'],
  );
}

/**
 * Implements form process callback for a payment_method element.
 *
 * @see payment_element_info()
 */
function payment_form_process_method(array $element, array &$form_state, array &$form) {
  $form_state['payment_parents'] = $element['#parents'];
  $payment = $form_state['payment'];

  $element['#tree'] = TRUE;
  $pmid_options = array();
  $pmids = empty($element['#pmids']) ? FALSE : $element['#pmids'];
  foreach ($payment->availablePaymentMethods(entity_load('payment_method', $pmids)) as $payment_method) {
    // Cast the PMID to a string or the AJAX callback won't work.
    $pmid_options[(string) $payment_method->pmid] = $payment_method->title_generic;
  }
  $form['#prefix'] = '<div id="payment-method-wrapper">';
  $form['#suffix'] = '</div>';
  $element['pmid'] = array(
    '#type' => 'select',
    '#title' => isset($element['#title']) ? $element['#title'] : NULL,
    '#options' => $pmid_options,
    '#default_value' => isset($payment->method) ? $payment->method->pmid : NULL,
    '#empty_value' => 'select',
    '#required' => $element['#required'],
    '#ajax' => array(
      'callback' => 'payment_form_process_method_submit_ajax_callback',
      'effect' => 'fade',
      'event' => 'change',
      'wrapper' => 'payment-method-wrapper',
    ),
    // Disable the selector for non-JS pages. This means that if we're
    // executing an AJAX callback, _triggering_element_name is set and we leave
    // the element enabled.
    '#disabled' => !empty($payment->method) && !isset($form_state['input']['_triggering_element_name']),
    '#attached' => array(
      'js' => array(drupal_get_path('module', 'payment') . '/js/payment.js'),
    ),
    '#id' => 'payment-method-pmid',
  );
  $element += payment_form_process_method_controller_payment_configuration($element, $form_state);

  // The element itself has no input, only its children, so mark it not
  // required to prevent validation errors.
  $element['#required'] = FALSE;

  return $element;
}

/**
 * Implements form validate callback for a payment_method element.
 */
function payment_form_process_method_validate(array $element, array &$form_state) {
  $pmid = drupal_array_get_nested_value($form_state['values'], array_merge($element['#parents'], array('pmid')));
  if ($pmid) {
    $payment = $form_state['payment'];
    if (!$payment->method || $payment->method->pmid != $pmid) {
      $form_state['rebuild'] = TRUE;
      $payment->method = entity_load_single('payment_method', $pmid);
      payment_method_controller_form_callback($element, $form_state, $payment->method->controller, 'payment', 'validate');
    }
  }
}

/**
 * Implements form submit callback for a payment_method element.
 */
function payment_form_process_method_submit(array $form, array &$form_state) {
  $pmid = drupal_array_get_nested_value($form_state['values'], array_merge($form_state['payment_parents'], array('pmid')));
  unset($pmid);
  $form_state['payment']->method = NULL;
  $form_state['rebuild'] = TRUE;
}

/**
 * Return a payment's method controller's form elements to configure a payment.
 *
 * @param array $element
 *   The payment_method element that is the parent of the elements to create.
 * @param array $form_state
 *
 * @return array
 */
function payment_form_process_method_controller_payment_configuration(array $element, array &$form_state) {
  $payment = $form_state['payment'];

  $elements = array();
  if ($payment->method) {
    $elements['change'] = array(
      '#type' => 'submit',
      '#value' => t('Change payment method'),
      '#submit' => array('payment_form_process_method_submit'),
      '#limit_validation_errors' => array(),
      '#attributes' => array(
        'class' => array('js-hide')
      ),
    );
    $elements['payment_method_controller_payment_configuration'] = payment_method_controller_form_callback($element, $form_state, $payment->method->controller, 'payment', 'build');
  }

  return $elements;
}

/**
 * Implements form AJAX callback for a payment_method element.
 */
function payment_form_process_method_submit_ajax_callback(array $form, array &$form_state) {
  return $form;
}

/**
 * Call one of a payment method controller's form callbacks.
 *
 * @param array $form
 * @param array $form_state
 * @param PaymentMethodController $controller
 * @param string $callback
 *   Either "payment" or "payment_method".
 * @param string $operation
 *   Either "build" or "validate".
 *
 * @return mixed
 *   The callback's return value.
 */
function payment_method_controller_form_callback(array $form, array &$form_state, PaymentMethodController $controller, $callback, $operation) {
  $property = $callback . '_configuration_form_elements_callback';
  switch ($operation) {
    case 'build':
      $function = $controller->$property;
      break;
    case 'validate':
      $function = $controller->$property . '_validate';
      break;
  }
  if (function_exists($function)) {
    return $function($form, $form_state);
  }
}

/**
 * Returns a hierarchical representation of payment statuses.
 *
 * @return array
 *   A possibly infinitely nested associative array. Keys are statuses and
 *   values are arrays of similar structure as this function's return value.
 */
function payment_status_hierarchy() {
  static $hierarchy = NULL;

  if (is_null($hierarchy)) {
    $parents = $children = array();
    $statuses_info = payment_statuses_info();
    uasort($statuses_info, 'payment_payment_status_sort_title');
    foreach ($statuses_info as $status_info) {
      $children[$status_info->parent][] = $status_info->status;
      if (!$status_info->parent) {
        $parents[] = $status_info->status;
      }
    }
    $hierarchy = _payment_status_hierarchy_level($parents, $children);
  }

  return $hierarchy;
}

/**
 * Helper function for payment_status_hierarchy().
 *
 * @param array $parents
 *   An array with payment statuses that are part of the same hierarchy level.
 * @param array $children
 *   Keys are payment statuses. Values are arrays with those statuses' child
 *   statuses.
 *
 * @return array
 *   The return value is identical to that of payment_status_hierarchy().
 */
function _payment_status_hierarchy_level(array $statuses, array $children) {
  $hierarchy = array();
  foreach ($statuses as $status) {
    $hierarchy[$status] = isset($children[$status]) ? _payment_status_hierarchy_level($children[$status], $children) : array();
  }

  return $hierarchy;
}

/**
 * Return payment statuses for use in form elements.
 *
 * @return array
 *   Keys are payment statuses. Values are status titles, prefixed with dashes
 *   to show hierarchy.
 */
function payment_status_options() {
  return _payment_status_options_level(payment_status_hierarchy(), 0);
}

/**
 * Helper function for payment_status_options().
 *
 * @param array $hierarchy
 *   A payment status hierarchy as returned by payment_status_hierarchy().
 * @param integer $depth
 *   The depth of $hierarchy's top-level items as seen from the original
 *   hierarchy's root (this function is recursive), starting with 0.
 *
 * @return array
 *   The return value is identical to that of payment_status_options().
 */
function _payment_status_options_level(array $hierarchy, $depth) {
  $options = array();
  $prefix = $depth ? str_repeat('-', $depth) . ' ' : '';
  foreach ($hierarchy as $status => $children) {
    $options[$status] = $prefix . payment_status_info($status)->title;
    $options += _payment_status_options_level($children, $depth + 1);
  }

  return $options;
}