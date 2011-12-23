<?php

/**
 * @file
 * Payment hooks.
 */

/**
 * Implements hook_payment_status_info().
 */
function payment_payment_status_info() {
  $status[PAYMENT_STATUS_MONEY_TRANSFERRED] = array(
    'title' => t('Money has been transferred'),
  );
  $status[PAYMENT_STATUS_MONEY_NOT_TRANSFERRED] = array(
    'title' => t('No money has been transferred'),
  );
  $status[PAYMENT_STATUS_UNKNOWN] = array(
    'title' => t('Unknown'),
  );
  $status[PAYMENT_STATUS_PENDING] = array(
    'title' => t('Pending'),
    'base status' => PAYMENT_STATUS_MONEY_NOT_TRANSFERRED,
  );
  $status[PAYMENT_STATUS_SUCCESS] = array(
    'title' => t('Completed'),
    'base status' => PAYMENT_STATUS_MONEY_TRANSFERRED,
  );
  $status[PAYMENT_STATUS_FAILED] = array(
    'title' => t('Failed'),
    'base status' => PAYMENT_STATUS_MONEY_NOT_TRANSFERRED,
  );

  return $status;
}

/**
 * Implements hook_payment_method_controller_info().
 */
function payment_payment_method_controller_info() {
  return array('PaymentMethodControllerUnavailable');
}