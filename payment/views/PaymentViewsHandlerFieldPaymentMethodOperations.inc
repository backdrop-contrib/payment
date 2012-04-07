<?php

/**
 * Views field handler for a payment method's operations links.
 */
class PaymentViewsHandlerFieldPaymentMethodOperations extends views_handler_field {

  /**
   * Implements views_handler_field::render().
   */
  function render($values) {
    $pmid = $this->get_value($values);
    $links = menu_contextual_links('payment', 'admin/config/services/payment/method', array($pmid));

    foreach (array('enable', 'disable', 'clone') as $op) {
      if (isset($links['payment-' . $op])) {
        $links['payment-' . $op]['href'] .= '/' . drupal_get_token('payment_method_administer');
      }
    }

    return theme('links__ctools_dropbutton', array(
      'links' => $links,
      'attributes' => array(
        'class' => array('links', 'inline', 'operations'),
      ),
    ));
  }
}
