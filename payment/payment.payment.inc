<?php

/**
 * @file
 * Payment hooks.
 */

/**
 * Implements hook_payment_status_info().
 */
function payment_payment_status_info() {
  $status[PAYMENT_STATUS_NEW] = array(
    'terminal' => FALSE,
    'title' => t('new'),
  );
  $status[PAYMENT_STATUS_PENDING] = array(
    'terminal' => FALSE,
    'title' => t('pending'),
  );
  $status[PAYMENT_STATUS_SUCCESS] = array(
    'title' => t('completed'),
  );
  $status[PAYMENT_STATUS_FAILED] = array(
    'title' => t('failed'),
  );
  $status[PAYMENT_STATUS_UNKNOWN] = array(
    'title' => t('unknown'),
  );

  return $status;
}

/**
 * Implements hook_payment_method_controller_info().
 */
function payment_payment_method_controller_info() {
  return array('PaymentMethodControllerUnavailable');
}