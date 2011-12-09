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
    $links = array(
      array(
        'title' => 'edit',
        'href' => 'admin/config/services/payment/method/' . $pmid . '/edit',
      ),
      array(
        'title' => 'delete',
        'href' => 'admin/config/services/payment/method/' . $pmid . '/delete',
      ),
    );

    return theme('links', array(
      'links' => $links,
      'attributes' => array(
        'class' => array('links', 'inline', 'operations'),
      ),
    ));
  }
}
