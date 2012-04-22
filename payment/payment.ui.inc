<?php

/**
 * @file
 * The Payment user interface.
 */

/**
 * Menu title callback: return a payment's title.
 *
 * @param $payment Payment
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
 * @param $modules array
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
 * @param $payment Payment
 *
 * @return array
 */
function payment_page_payment_view(Payment $payment) {
  return entity_view('payment', array($payment));
}

/**
 * Implements form build callback: the payment add/edit form.
 *
 * @param $form array
 * @param $form_state array
 * @param $payment Payment
 *
 * @return array
 *   A render array.
 */
function payment_form(array $form, array &$form_state, Payment $payment) {
  $form_state['payment'] = $payment;
  $options = array();
  foreach (payment_statuses_info() as $status_info) {
    $options[$status_info->status] = $status_info->title;
  }
  asort($options);
  $form['status'] = array(
    '#access' => !empty($payment->pid),
    '#type' => 'select',
    '#title' => t('Status'),
    '#options' => $options,
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
      $items[] = array(
        'title' => $controller->title,
        'href' => 'admin/config/services/payment/method/add/' . $controller->name,
        'description' => $controller->description,
        'localized_options' => array(),
      );
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
 * Create a blank payment method and return its payment form.
 *
 * @param $controller_class_name string
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
 * @see payment_method_form()
 *
 * @param $payment_method PaymentMethod
 */
function payment_form_payment_method(array $form, array &$form_state, PaymentMethod $payment_method) {
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
    // '#title' => t('Machine name'),
    '#default_value' => $payment_method->name,
    '#maxlength' => 255,
    '#required' => TRUE,
    '#machine_name' => array(
      'source' => array('title_specific'),
      'exists' => 'payment_method_name_exists',
    ),
  );
  $form['title_generic'] = array(
    '#type' => 'textfield',
    '#title' => t('Title (generic)'),
    '#description' => t('The generic title is often only displayed to people such as payers who only need to know the generic payment method that is used, for instance <em>Paypal</em>. Defaults to the specific title.'),
    '#default_value' => $payment_method->title_generic,
    '#maxlength' => 255,
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
    );
  }

  return $form;
}

/**
 * Implements form validate callback for payment_form_payment_method().
 */
function payment_form_payment_method_validate(array $form, array &$form_state) {
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
  $parents = $children = array();
  $statuses_info = payment_statuses_info();
  uasort($statuses_info, 'payment_payment_status_sort_title');
  foreach ($statuses_info as $status_info) {
    foreach ($status_info->parents as $parent_status) {
      $children[$parent_status][] = $status_info->status;
    }
    if (!$status_info->parents) {
      $parents[] = $status_info->status;
    }
  }
  $rows = array();
  _payment_page_status_rows($parents, $children, $rows);
  $header = array(t('Title'));

  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
  ));
}

/**
 * Helper function to build the rows for the table in payment_page_status().
 *
 * @see payment_page_status()
 *
 * @param $statuses array
 *   The statuses for which to build table rows.
 * @param $children array
 *   Keys are payment statuses, values are the statuses that are the keys's
 *   children.
 * @param $rows array
 *   The table rows to which to add new ones.
 * @param $depth integer
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
 * Implements theme callback: a payment status for the overview page.
 *
 * @param $variables array
 *   An associative array containing:
 *   - status_info: the PaymentStatusInfo object to theme.
 *   - depth: the status' depth within the status hierarchy.
 */
function theme_payment_page_status_row(array $variables) {
  $status_info = payment_status_info($variables['status']);

  return theme('indentation', array(
    'size' => $variables['depth'],
  )) . '<div style="float: left;">' . $status_info->title . '<div class="description">' . $status_info->description . '</div></div>';
}

/**
 * Implements uasort() callback to sort PaymentStatusInfo objects by title.
 */
function payment_payment_status_sort_title(PaymentStatusInfo $status_a, PaymentStatusInfo $status_b) {
  return strcmp($status_a->title, $status_b->title);
}

function payment_method_name_exists($name) {
  return (bool) entity_load_single('payment_method', $name);
}

/**
 * Return a render array containing a Payment's line items.
 *
 * @param $payment Payment
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
 * @param $payment Payment
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
 * @param string $op
 *   Either 'enable' or 'disable'.
 * @param PaymentMethod $payment_method
 *
 * @return NULL
 */
function payment_page_method_enable_disable($op, PaymentMethod $payment_method, $token) {
  if ($token != drupal_get_token('payment_method_administer')) {
    drupal_access_denied();
    exit;
  }

  switch ($op) {
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

function payment_page_method_clone(PaymentMethod $payment_method, $token) {
  if ($token != drupal_get_token('payment_method_administer')) {
    drupal_access_denied();
    exit;
  }

  $payment_method = clone $payment_method;
  $payment_method->pmid = 0;
  $payment_method->name = '';

  return payment_method_form($payment_method);
}

/**
 * Implements form process callback for a payment_amount element.
 */
function payment_form_process_amount(array $element) {
  $element['#type'] = 'textfield';
  $element['#field_prefix'] = $element['#currency_code'];
  $description = NULL;
  if ($element['#minimum_amount'] > 0) {
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

  // Count the number of decimal marks.
  $count_fragments = count(preg_split('#[.,]#', $value));

  // Check for invalid characters.
  if (preg_match('#[^\d.,]#', $value)) {
    form_error($element, t('The amount can only consist of decimals and one decimal mark.'));
  }
  // Check for multiple decimal marks.
  elseif ($count_fragments > 2) {
    form_error($element, t('The amount can only have one decimal mark.'));
  }
  else {
    // Convert the value to a float.
    $amount = (float) $value;

    // Confirm the amount lies within the allowed range.
    if ($amount < $element['#minimum_amount']) {
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