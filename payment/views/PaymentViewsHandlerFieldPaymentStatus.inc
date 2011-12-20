<?php

/**
 * Views field handler for a human-readable payment status.
 */
class PaymentViewsHandlerFieldPaymentStatus extends views_handler_field {

  /**
   * Implements views_handler_field::render().
   */
  function render($values) {
    $status_info = payment_status_info($this->get_value($values), TRUE);

    return $status_info['title'];
  }
}
