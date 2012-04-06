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

  if (empty($form_state['rebuild']) && $payment->method && !$payment->method->validate($payment)) {
    form_set_error('payment_method', t('The payment did not pass validation.'));
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

  return payment_method_form($payment_method);
}

/**
 * Return a payment method's settings form.
 *
 * @param $payment_method PaymentMethod
 *
 * @return array
 *   A Drupal form.
 */
function payment_method_form(PaymentMethod $payment_method) {
  return drupal_get_form('payment_form_payment_method_' . $payment_method->controller->payment_method_form_id, $payment_method);
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
 * Implements form submit callback for payment_form_payment_method().
 */
function payment_form_payment_method_submit(array $form, array &$form_state) {
  $values = $form_state['values'];
  $payment_method = $form_state['payment_method'];
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
 *   
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