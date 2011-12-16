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
  $actions['payment_rules_payment_status_change'] = array(
    'label' => t('Change the payment status'),
    'parameter' => array(
      'payment' => array(
        'type' => 'payment',
        'label' => t('Label'),
        'save' => TRUE,
      ),
      'status' => array(
        'type' => 'list<text>',
        'label' => t('Status'),
        'options list' => 'payment_rules_payment_status_change_options_list',
      ),
    ),
    'group' => t('Payment'),
    'base' => 'payment_rules_payment_status_change',
  );

  return $actions;
}

/**
 * Implements Rules action callback for payment_rules_payment_status_change.
 */
function payment_rules_payment_status_change(Payment $payment, $status_data) {
  $payment->setStatus(reset($status_data));
  enitty_save('payment', $payment);
}

/**
 * Implements Rules options list callback for
 * payment_rules_payment_status_change.
 */
function payment_rules_payment_status_change_options_list() {
  $options = array();
  foreach (payment_statuses_info() as $status => $status_info) {
    $options[$status] = $status_info['title'];
  }
  asort($options);

  return $options;
}