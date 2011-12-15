<?php

/**
 * @file
 * The Payment interface.
 */

/**
 * Menu title callback: return a payment's title.
 *
 * @param $payment Payment
 *
 * @return string
 */
function paymentui_payment_title(Payment $payment) {
  return t('Payment !pid', array(
    '!pid' => $payment->pid,
  ));
}

/**
 * Menu page callback: show a payment.
 *
 * @param $payment Payment
 *
 * @return array
 */
function paymentui_page_payment_view(Payment $payment) {
  return entity_view('payment', array($payment));
}