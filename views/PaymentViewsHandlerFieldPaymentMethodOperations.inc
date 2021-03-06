<?php

/**
 * @file
 * A views field handler. See payment_views_data().
 */

/**
 * Views field handler for a payment method's operations links.
 */
class PaymentViewsHandlerFieldPaymentMethodOperations extends views_handler_field {

  /**
   * Implements views_handler_field::render().
   */
  function render($values) {
    $token = backdrop_get_token('payment');
    $pmid = $this->get_value($values);
    $links = menu_contextual_links('payment', 'admin/config/services/payment/method', array($pmid, '', $token));

    foreach (array('enable', 'disable') as $operation) {
      if (isset($links['payment-' . $operation])) {
        $links['payment-' . $operation]['href'] .= '/' . $token;
      }
    }
    $dropbutton = array(
      '#type' => 'dropbutton',
      '#links' => $links,
    );

    return backdrop_render($dropbutton);

  }
}
