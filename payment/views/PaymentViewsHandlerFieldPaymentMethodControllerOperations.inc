<?php

/**
 * Field handler to present an order's operations links.
 */
class PaymentViewsHandlerFieldPaymentMethodControllerOperations extends views_handler_field {

  /**
   * Implements views_handler_field::render().
   */
  function render($values) {
    $pmid = $this->get_value($values);
    $links = menu_contextual_links('payment', 'admin/config/services/payment/method', array($pmid));

    return theme('links', array(
      'links' => $links,
      'attributes' => array(
        'class' => array('links', 'inline', 'operations'),
      ),
    ));
  }
}
