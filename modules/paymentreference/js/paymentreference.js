(function($) {
  /**
   * Refresh this window's opener's payment references.
   */
  $(document).ready(function() {
    if (window.opener && window.opener.Backdrop.PaymentreferenceRefreshButtons) {
      window.opener.Backdrop.PaymentreferenceRefreshButtons();
    }
  });

  /**
   * Convert "close this window" messages to links.
   */
  Backdrop.behaviors.PaymentreferenceWindowCloseLink = {
    attach: function(context) {
      if (window.opener) {
        $('span.paymentreference-window-close').each(function() {
          $(this).replaceWith('<a href="#" class="paymentreference-window-close">' + this.innerHTML + '</a>');
        });
        $('a.paymentreference-window-close').bind('click', function() {
          window.opener.focus();
          window.close();
        });
      }
    }
  }

  /**
   * Refresh all payment references.
   */
  Backdrop.PaymentreferenceRefreshButtons = function() {
    $('.paymentreference-refresh-button').each(function() {
      if (!Backdrop.settings.PaymentreferencePaymentAvailable[Backdrop.settings.ajax[this.id].wrapper]) {
        $(this).trigger('mousedown');
      }
    });
  }

  /**
   * Set an interval to refresh all payment references.
   */
  setInterval(Backdrop.PaymentreferenceRefreshButtons, 30000);
})(jQuery);