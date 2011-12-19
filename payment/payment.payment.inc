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
    'title' => t('New'),
  );
  $status[PAYMENT_STATUS_PENDING] = array(
    'title' => t('Pending'),
  );
  $status[PAYMENT_STATUS_SUCCESS] = array(
    'title' => t('Completed'),
  );
  $status[PAYMENT_STATUS_FAILED] = array(
    'title' => t('Failed'),
  );
  $status[PAYMENT_STATUS_UNKNOWN] = array(
    'title' => t('Unknown'),
  );

  return $status;
}

/**
 * Implements hook_payment_method_controller_info().
 */
function payment_payment_method_controller_info() {
  return array('PaymentMethodControllerUnavailable');
}