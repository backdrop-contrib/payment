<?php

/**
 * @file
 * Rules integration.
 */

/**
 * Implements hook_rules_event_info().
 */
function payment_rules_event_info() {
  $event['payment_status_change'] = array(
    'label' => t("After changing a payment's status"),
    'group' => t('Payment'),
    'variables' => array(
      'payment' => array(
        'type' => 'payment',
        'label' => t('Payment'),
      ),
      'payment_old_status' => array(
        'type' => 'text',
        'label' => t("The payment's old status"),
      ),
    ),
  );

  return $event;
}

/**
 * Implements hook_rules_action_info().
 */
function payment_rules_action_info() {
  $actions['payment_rules_action_payment_set_status'] = array(
    'label' => t('Change the payment status'),
    'parameter' => array(
      'payment' => array(
        'type' => 'payment',
        'label' => t('Label'),
      ),
      'status' => array(
        'type' => 'text',
        'label' => t('Status'),
        'options list' => 'payment_rules_options_list_payment_set_status',
      ),
    ),
    'group' => t('Payment'),
  );

  return $actions;
}

/**
 * Implements hook_rules_condition_info().
 */
function payment_rules_condition_info() {
  $conditions['payment_rules_condition_payment_has_status'] = array(
    'label' => t('Payment has status'),
    'parameter' => array(
      'payment' => array(
        'label' => t('Payment'),
        'type' => 'payment',
        'restriction' => 'selector',
      ),
      'payment_statuses' => array(
        'type' => 'list<text>',
        'label' => t('Payment status'),
        'options list' => 'payment_rules_options_list_payment_has_status',
        'restriction' => 'input',
      ),
    ),
    'group' => t('Payment'),
  );
  $conditions['payment_rules_condition_payment_uses_payment_method'] = array(
    'label' => t('Payment uses payment method'),
    'parameter' => array(
      'payment' => array(
        'label' => t('Payment'),
        'type' => 'payment',
        'restriction' => 'selector',
      ),
      'pmids' => array(
        'type' => 'list<text>',
        'label' => t('Payment methods'),
        'options list' => 'payment_rules_options_list_payment_uses_payment_method',
        'restriction' => 'input',
      ),
    ),
    'group' => t('Payment'),
  );
  $conditions['payment_rules_condition_payment_uses_payment_method_type'] = array(
    'label' => t('Payment uses payment method of type'),
    'parameter' => array(
      'payment' => array(
        'label' => t('Payment'),
        'type' => 'payment',
        'restriction' => 'selector',
      ),
      'payment_method_controller_class_names' => array(
        'type' => 'list<text>',
        'label' => t('Payment method types'),
        'options list' => 'payment_rules_options_list_payment_uses_payment_method_type',
        'restriction' => 'input',
      ),
    ),
    'group' => t('Payment'),
  );

  return $conditions;
}

/**
 * Implements Rules condition callback: check whether a payment's status
 * matches one of the selected statuses.
 *
 * @param $payment Payment
 *   The payment to check the condition for.
 * @param $statuses array
 *   An array with payment statuses.
 *
 * @return boolean
 */
function payment_rules_condition_payment_has_status(Payment $payment, array $payment_statuses) {
  return in_array($payment->status, $payment_statuses) || in_array('payment_base_' . payment_status_base($payment->status, TRUE), $payment_statuses);
}

/**
 * Implements Rules options list: list a payment's available statuses.
 */
function payment_rules_options_list_payment_has_status() {
  $options = array();
  $statuses_info = payment_statuses_info();
  foreach ($statuses_info as $status => $status_info) {
    $options[$status] = $status_info['title'];
    if ($status == $status_info['base status']) {
      $options['payment_base_' . $status] = $status_info['title'] . ' (' . t('or any derived status') . ')';
    }
    else {
      $options[$status] .= ' (' . t('derived from "@base_status_title"', array(
        '@base_status_title' => $statuses_info[$status_info['base status']]['title'],
      )) . ')';
    }
  }
  asort($options);

  return $options;
}

/**
 * Implements Rules action callback for payment_rules_payment_status_change.
 */
function payment_rules_action_payment_set_status(Payment $payment, $status_data) {
  $payment->setStatus(reset($status_data));
  enitty_save('payment', $payment);
}

/**
 * Implements Rules options list callback for
 * payment_rules_payment_status_change.
 */
function payment_rules_options_list_payment_set_status() {
  $options = array();
  foreach (payment_statuses_info() as $status => $status_info) {
    $options[$status] = $status_info['title'];
  }
  asort($options);

  return $options;
}

/**
 * Implements Rules options list callback for
 * payment_rules_condition_payment_uses_payment_method.
 */
function payment_rules_options_list_payment_uses_payment_method() {
  $options = array();
  foreach (entity_load('payment_method') as $payment_method) {
    $options[$payment_method->pmid] = $payment_method->title_specific;
  }

  return $options;
}

/**
 * Implements Rules condition callback: check whether a payment uses one of the
 * selected payment methods.
 *
 * @param $payment Payment
 *   The payment to check the condition for.
 * @param $pmids array
 *   An array with payment method IDs.
 *
 * @return boolean
 */
function payment_rules_condition_payment_uses_payment_method(Payment $payment, array $pmids) {
  return in_array($payment->method->pmid, $pmids);
}

/**
 * Implements Rules options list callback for
 * payment_rules_condition_payment_uses_payment_method_type.
 */
function payment_rules_options_list_payment_uses_payment_method_type() {
  $options = array();
  foreach (payment_method_controller_load_multiple() as $payment_method_controller) {
    $options[$payment_method_controller->name] = $payment_method_controller->title;
  }

  return $options;
}

/**
 * Implements Rules condition callback: check whether a payment uses one of the
 * selected payment methods types.
 *
 * @param $payment Payment
 *   The payment to check the condition for.
 * @param $payment_method_controller_class_names array
 *   An array with class names of payment method controllers.
 *
 * @return boolean
 */
function payment_rules_condition_payment_uses_payment_method_type(Payment $payment, array $payment_method_controller_class_names) {
  return in_array($payment->method->controller->name, $payment_method_controller_class_names);
}