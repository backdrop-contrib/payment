<?php

/**
 * Views field handler for a payment's total amount.
 */
class PaymentViewsHandlerFieldPaymentTotalAmount extends views_handler_field {

  /**
   * Implements views_handler_field::query().
   */
  function query() {
    $join = new views_join();
    $join->construct('payment_amount', 'payment', 'pid', 'pid');
    $this->query->add_relationship('payment_amount', $join, 'payment');
    $this->query->add_field(NULL, 'SUM(amount)', 'amount_total');
  }

  /**
   * Implements views_handler_field::pre_render().
   */
  function pre_render(&$values) {
    // Cast the amounts to their original types, because some backends
    // store/return all variables as strings.
    foreach ($values as &$value) {
      $value->amount_total = (float) $value->amount_total;
    }
  }

  /**
   * Implements views_handler_field::render().
   */
  function render($values) {
    return payment_amount_human_readable($values->amount_total);
  }
}
