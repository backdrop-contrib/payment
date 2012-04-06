<?php

/**
 * Views field handler for a payment method's enabled status machine name.
 */
class PaymentViewsHandlerFieldPaymentMethodEnabledMachineName extends views_handler_field {

  /**
   * Implements views_handler_field::render().
   */
  function render($values) {
    return $this->get_value($values) ? 'enabled' : 'disabled';
  }
}
