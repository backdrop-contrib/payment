<?php

class PaymentViewsHandlerFieldPaymentMethodControllerTitle extends views_handler_field {

  /**
   * Implements views_handler_field::render().
   */
  function render($values) {
    return payment_method_controller_load($this->get_value($values))->title;
  }
}
