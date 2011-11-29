<?php

/**
 * @file
 * Hook documentation.
 */

/**
 * Define payment statuses.
 *
 * @return array
 *   An array where keys are payment status names and values are keyed arrays
 *   with information about that specific status:
 *   - base status (string)
 *     Another status this status is based on. Optional. Statuses may be based
 *     on any other status, but because modules depend on certain bases
 *     statuses, you are advised to use PAYMENT_STATUS_NEW,
 *     PAYMENT_STATUS_PENDING, PAYMENT_STATUS_SUCCESS, PAYMENT_STATUS_FAILED,
 *     or PAYMENT_STATUS_UNKNOWN. Leave empty if this status is not based on
 *     another one.
 *   - description (string)
 *     A human-readable description of the status. Optional. Defaults to an
 *     empty string.
 *   - terminal (boolean)
 *     Whether this status means the payment is terminal, e.g. it is not being
 *     processed any further. Optional. Defaults to TRUE.
 *   - title (string)
 *     A human-readable title. Required.
 */
function hook_payment_status_info() {
  $status_info[PAYMENT_STATUS_FOO] = array(
    'base status' => PAYMENT_STATUS_PENDING,
    'description' => t('Foo payments are still being processed by bar to guarantee their authenticity.'),
    'terminal' => FALSE,
    'title' => t('foo to the bar'),
  );

  return $status_info;
}

/**
 * Define payment method controllers.
 *
 * @return array
 *   An array with the names of payment method controller classes.
 */
function hook_payment_method_controller_info() {
  return array(
    'DummyPaymentMethodController',
    'CashOnDeliveryPaymentMethodController',
  );
}

/**
 * Act on a payment's changed status.
 *
 * @see Payment::setStatus()
 *
 * @param $payment Payment
 * @param $old_status string
 *   The status the payment had before it was changed.
 *
 * @return NULL
 */
function hook_payment_status_change(Payment $payment, $old_status) {
  // Notify the site administrator, for instance.
}