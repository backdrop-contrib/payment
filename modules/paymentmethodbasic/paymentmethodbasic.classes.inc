<?php

/**
 * @file
 * A basic payment method controller.
 */

/**
 * This controller always executes payments successfully, does not require
 * payment-specific information and does not transfer any money. It can be
 * useful to create a "collect on delivery" payment method, for instance.
 */
class PaymentMethodBasicController extends PaymentMethodController {

  public $controller_data_defaults = array(
    'message' => '',
    'status' => PAYMENT_STATUS_SUCCESS,
    'text_format' => '',
  );

  public $payment_method_configuration_form_elements_callback = 'paymentmethodbasic_payment_method_configuration_form_elements';

  public $payment_configuration_form_elements_callback = 'paymentmethodbasic_payment_configuration_form_elements';

  function __construct() {
    $this->title = t('Basic');
    $this->description = t("A 'dumb' payment method type that always successfully executes payments, but never actually transfers money. It can be useful for <em>collect on delivery</em>, for instance.");
  }

  /**
   * Implements PaymentMethodController::execute().
   */
  function execute(Payment $payment) {
    $payment->setStatus(new PaymentStatusItem($payment->method->controller_data['status']));
  }

  /**
   * {@inheritdoc}
   */
  public function validate(Payment $payment, PaymentMethod $payment_method, $strict) {
    // This payment method controller does not have a minimum amount, so when
    // the parent throws such an exception, catch it and do nothing.
    try {
      parent::validate($payment, $payment_method, $strict);
    }
    catch (PaymentValidationAmountBelowMinimumException $e) {
    }
  }
}
