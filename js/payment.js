(function($) {
  /**
   * Enable the payment method select element.
   */
  Backdrop.behaviors.PaymentMethodSelector = {
    attach: function(context) {
	    $('#payment-method-pmid').attr('disabled', false);
    }
  }
})(jQuery);