<?php

/**
 * Views field handler for a human-readable country name.
 */
class PaymentViewsHandlerFieldPaymentBillingCountryHumanReadable extends views_handler_field {

  /**
   * Implements views_handler_field::render().
   */
  function render($values) {
    require_once('includes/iso.inc');
    $countries = _country_get_predefined_list();

    return $countries[$this->get_value($values)];
  }
}
