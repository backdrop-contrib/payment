<?php

/**
 * @file
 * Hook documentation.
 */

/**
 * Defines payment statuses.
 *
 * @return array
 *   An array with PaymentStatusInfo objects.
 */
function hook_payment_status_info() {
  return array(
    new PaymentStatusInfo(array(
      'description' => t('Foo payments are still being processed by Bar to guarantee their authenticity.'),
      'status' => PAYMENT_STATUS_FOO,
      'parents' => array(PAYMENT_STATUS_PENDING),
      'title' => t('Pending (waiting for Bar authentication)'),
    )),
  );
}

/**
 * Alters payment statuses.
 *
 * @param $statuses_info array
 *   An array with PaymentStatusInfo objects.
 *
 * @return NULL
 */
function hook_payment_status_info_alter(&$statuses_info) {
  $statuses_info[PAYMENT_STATUS_FAILED]->title = 'Something went wrong!';
}

/**
 * Defines payment method controllers.
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
 * Alters payment method controllers.
 *
 * @param array
 *   An array with the names of payment method controller classes.
 */
function hook_payment_method_controller_info_alter(&$controllers_info) {
  // Remvove a payment method controller.
  $reversed = array_reverse($controllers_info);
  unset($reversed['FooPaymentMethodController']);
  $controllers_info = array_reverse($reversed);
}

/**
 * Executes when a payment status has been changed.
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

/**
 * Executes right before a payment is executed. This is the place to
 * programmatically alter payments.
 *
 * @see Payment::execute()
 *
 * @param $payment Payment
 *
 * @return NULL
 */
function hook_payment_pre_execute(Payment $payment) {
  // Add a payment method processing fee.
  $payment->amount += 5.50;
}