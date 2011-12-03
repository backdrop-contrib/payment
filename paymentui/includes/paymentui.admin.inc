<?php

/**
 * @file
 * The Payment administration interface.
 */

/**
 * List all payments.
 *
 * @return string
 */
function paymentui_payment_list() {
  global $pager_page_array, $pager_total, $pager_total_items;

  $page = isset($_GET['page']) ? $_GET['page'] : '';

  // Build the pager. We use Drupal's built-in paging system, but don't use any
  // of the API functions, because they are too SQL-dependent.
  $query = new EntityFieldQuery();
  $count = $query->entityCondition('entity_type', 'payment')
    ->count()
    ->execute();
  $pager_page_array = explode(',', $page);
  $pager_total_items[0] = $count;
  $pager_total[0] = ceil($count / 20);
  $pager = theme('pager');

  // Build the table.
  $header = array(
    array(
      'data' => t('Last update'),
      'field' => 'updated',
      'sort' => 'desc',
    ),
    array(
      'data' => t('Amount'),
      'field' => 'amount',
    ),
    array(
      'data' => t('Description'),
      'field' => 'description',
    ),
    array(
      'data' => t('Name'),
      'field' => 'name',
    ),
    array(
      'data' => t('City'),
      'field' => 'city',
    ),
    array(
      'data' => t('Account number'),
      'field' => 'account_number',
    ),
    t('Status'),
  );
  $sort_field = 'updated';
  $order = 'desc';
  if (isset($_GET['order'])) {
    foreach ($header as $cell) {
      if ($cell['data'] == $_GET['order']) {
        $sort_field = $cell['field'];
        $order = isset($_GET['sort']) ? $_GET['sort'] : $cell['sort'];
      }
    }
  }
  $query = new EntityFieldQuery();
  $entities = $query->entityCondition('entity_type', 'payment')
    ->propertyOrderBy($sort_field, $order)
    ->range($page * 20, 20)
    ->execute();
  $payments = entity_load('payment', array_keys($entities['payment']));
  $rows = array();
  foreach ($payments as $payment) {
    $status_info = payment_status_info($payment->status);
    $rows[] = array(
      'data' => array(
        format_date($payment->updated, 'small'),
        $payment->currency_code . ' ' . payment_amount_human_readable($payment->amount),
        $payment->description,
        $payment->name,
        $payment->city,
        $payment->account_number,
        $status_info['title'],
      ),
    );
  }
  if (!$rows) {
    $rows[] = array(
      array(
        'data' => t('There are no payments yet.'),
        'colspan' => 7,
      ),
    );
  }

  return $pager . theme('table', array(
    'header' => $header,
    'rows' => $rows,
  )) . $pager;
}

/**
 * The merchant overview page.
 *
 * @return string
 */
function paymentui_page_payment_method_list() {
  $rows = array();
  foreach (entity_load('payment_method') as $payment_method) {
    $rows[] = array(
      check_plain($payment_method->title_specific),
      $payment_method->controller->title,
      l(t('edit'), 'admin/config/services/payment/method/edit/' . $payment_method->pmid),
      l(t('delete'), 'admin/config/services/payment/method/delete/' . $payment_method->pmid),
    );
  }

  if (!$rows) {
    $rows[] = array(array(
      'data' => t('There are no payment methods yet. <a href="@url">Add payment method</a>.', array(
        '@url' => url('admin/config/services/payment/method/add'),
      )),
      'colspan' => 4,
    ));
  }

  $header = array(
    t('Title'),
    t('Type'),
    array(
      'data' => t('Operations'),
      'colspan' => 2,
    ),
  );

  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
  ));
}

/**
 * Shows a page with controllers payment methods can be added for.
 *
 * @return string
 */
function payment_page_payment_method_add_select_controller() {
  $controllers = payment_method_controller_load_multiple();
  unset($controllers['PaymentMethodControllerDisabled']);
  if ($controllers) {
    $items = array();
    foreach ($controllers as $controller) {
      $items[] = array(
        'title' => $controller->title,
        'href' => 'admin/config/services/payment/method/add/' . get_class($controller),
        'description' => $controller->description,
        'localized_options' => array(),
      );
    }
    return theme('admin_block_content', array(
      'content' => $items,
    ));
  }
  else {
    return t('There are no payment method types available.');
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
function paymentui_payment_method_form_add($controller) {
  $payment_method = new PaymentMethod(array(
    'controller' => $controller,
  ));

  return paymentui_payment_method_form($payment_method);
}

/**
 * Return a payment method's settings form.
 *
 * @param $payment_method PaymentMethod
 *
 * @return array
 *   A Drupal form.
 */
function paymentui_payment_method_form(PaymentMethod $payment_method) {
  return drupal_get_form('payment_method_form_' . $payment_method->controller->payment_method_form_id, $payment_method);
}

/**
 * Implements form build callback: the payment method add/edit form.
 *
 * @see paymentui_forms()
 * @see paymentui_payment_method_form()
 *
 * @param $payment_method PaymentMethod
 */
function paymentui_form_payment_method(array $form, array &$form_state, PaymentMethod $payment_method) {
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
  $form['title_generic'] = array(
    '#type' => 'textfield',
    '#title' => t('Title (generic)'),
    '#description' => t('The generic title is often only displayed to people such as payers who only need to know the generic payment method that is used, for instance <em>Paypal</em>.'),
    '#default_value' => $payment_method->title_generic,
    '#maxlength' => 255,
    '#required' => TRUE,
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
      '#href' => 'admin/config/services/payment/method/delete/' . $payment_method->pmid,
    );
  }

  return $form;
}

/**
 * Implements form submit callback for paymentui_form_payment_method().
 */
function paymentui_form_payment_method_submit(array $form, array &$form_state) {
  $values = $form_state['values'];
  $payment_method = $form_state['payment_method'];
  $payment_method->title_specific = $values['title_specific'];
  $payment_method->title_generic = $values['title_generic'];
  entity_save('payment_method', $payment_method);
  $form_state['redirect'] = 'admin/config/services/payment/method';
  drupal_set_message(t('Payment method %title has been saved.', array(
    '%title' => $payment_method->title_specific,
  )));
}

/**
 * Implements form build callback: payment method deletion form.
 */
function paymentui_form_payment_method_delete(array $form, array &$form_state, PaymentMethod $payment_method) {
  $form_state['payment_method'] = $payment_method;

  return confirm_form($form, t('Do you really want to delete payment method %title?', array(
    '%title' => $form_state['payment_method']->title_specific,
  )), 'admin/config/services/payment/method', t('Existing payments that use this payment method will become unusable. This action cannot be undone.'));
}

/**
 * Implements form submit callback for paymentui_form_payment_method_delete().
 */
function paymentui_form_payment_method_delete_submit(array $form, array &$form_state) {
  $payment_method = $form_state['payment_method'];
  entity_delete('payment_method', $payment_method->pmid);
  $form_state['redirect'] = 'admin/config/services/payment/method';
  drupal_set_message(t('Payment method %title has been deleted.', array(
    '%title' => $payment_method->title_specific,
  )));
}

/**
 * Implements form build callback: global configuration form.
 */
function paymentui_form_global_configuration(array $form, array &$form_state) {
  $form['payment_display_log_entries'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display <em>Payment</em> log entries on screen'),
    '#default_value' => variable_get('payment_display_log_entries', TRUE),
  );

  return system_settings_form($form);
}