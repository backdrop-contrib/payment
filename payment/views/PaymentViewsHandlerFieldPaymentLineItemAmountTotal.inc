<?php

/**
 * @file
 * A views field handler. See payment_views_data().
 */

/**
 * Views field handler for a payment method's controller description.
 */
class PaymentViewsHandlerFieldPaymentLineItemAmountTotal  extends views_handler_field {

  /**
   * Implements views_handler_field::query().
   */
  function query() {
    $params = $this->options['group_type'] != 'group' ? array(
      'function' => $this->options['group_type'],
    ) : array();
    $this->query->add_field(NULL, '(amount * quantity * (tax_rate + 1))', 'amount_total', $params);
  }

  /**
   * Implements views_handler_field::render().
   */
  function render($values) {
    return $values->amount_total;
  }
}