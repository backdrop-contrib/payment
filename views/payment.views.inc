<?php

/**
 * @file
 * Views integration.
 */

/**
 * Implements hook_views_data().
 */
function payment_views_data() {
  $data['payment_line_item'] = array(
    'table' => array(
      'base' => array(
        'title' => t('Payment line item'),
      ),
      'group' => t('Payment line item'),
    ),
    // Field configuration.
    'amount' => array(
      'title' => t('Amount'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'float' => TRUE,
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
    ),
    'amount_total' => array(
      'title' => t('Total amount'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'float' => TRUE,
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
    ),
    'description' => array(
      'title' => t('Description'),
      'field' => array(
        'handler' => 'PaymentViewsHandlerFieldTranslatableString',
        'string_arguments_field' => 'description_arguments',
      ),
    ),
    'name' => array(
      'title' => t('Machine name'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_string',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
    ),
    'pid' => array(
      'title' => t('Payment ID'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
      'relationship' => array(
        'title' => t('Payment'),
        'base' => 'payment',
        'field' => 'pid',
        'handler' => 'views_handler_relationship',
        'label' => t('Payment'),
      ),
    ),
    'quantity' => array(
      'title' => t('Quantity'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
    ),
    'tax_rate' => array(
      'title' => t('Tax rate'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'float' => TRUE,
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
    ),
  );
  $data['payment'] = array(
    'table' => array(
      'base' => array(
        'title' => t('Payment'),
      ),
      'group' => t('Payment'),
    ),
  );
  $data['payment']['context'] = array(
    'title' => t('Context'),
    'help' => t('The payment context.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['payment']['currency_code'] = array(
    'title' => t('Currency code'),
    'help' => t('The payment currency.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['payment']['description'] = array(
    'title' => t('Currency code'),
    'help' => t('The payment currency.'),
    'field' => array(
      'handler' => 'PaymentViewsHandlerFieldTranslatableString',
      'string_arguments_field' => 'description_arguments',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['payment']['pid'] = array(
    'title' => t('PID'),
    'help' => t('The payment ID.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['payment']['pmid'] = array(
    'title' => t('PMID'),
    'help' => t('The PMID.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['payment']['psiid_first'] = array(
    'title' => t('First status item ID'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'float' => TRUE,
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'relationship' => array(
      'title' => t('First status item'),
      'base' => 'payment_status_item',
      'base field' => 'psiid',
      'handler' => 'views_handler_relationship',
      'label' => t('First status item'),
    ),
  );
  $data['payment']['psiid_last'] = array(
    'title' => t('Last status item ID'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'float' => TRUE,
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
    'relationship' => array(
      'title' => t('Last status item'),
      'base' => 'payment_status_item',
      'base field' => 'psiid',
      'handler' => 'views_handler_relationship',
      'label' => t('Last status item'),
    ),
  );
  $data['payment']['uid'] = array(
    'title' => t('Payment UID'),
    'help' => t('The UID this payment belongs to.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Payment methods.
  $data['payment_method'] = array(
    'table' => array(
      'base' => array(
        'title' => t('Payment method'),
      ),
      'group' => t('Payment method'),
    ),
  );
  $data['payment_method']['controller_class_name'] = array(
    'title' => t('Payment method controller class name'),
    'help' => t('Payment method controller class name.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['payment_method']['enabled'] = array(
    'title' => t('Payment method enabled'),
    'help' => t('Payment method enabled.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['payment_method']['module'] = array(
    'title' => t('Payment method module'),
    'help' => t('Payment method module.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['payment_method']['name'] = array(
    'title' => t('Payment method name'),
    'help' => t('Payment method name.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['payment_method']['pmid'] = array(
    'title' => t('Payment method PMID'),
    'help' => t('Payment method PMID.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument',
    ),
    'filter' => array(
      'handler' => 'PaymentViewsHandlerFilterPaymentMethodTitle',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['payment_method']['status'] = array(
    'title' => t('Payment method status'),
    'help' => t('Payment method status.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['payment_method']['title_generic'] = array(
    'title' => t('Payment method title generic'),
    'help' => t('Payment method title generic.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['payment_method']['title_specific'] = array(
    'title' => t('Payment method title specific'),
    'help' => t('Payment method title specific.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['payment_method']['uid'] = array(
    'title' => t('Payment method UID'),
    'help' => t('Payment method UID.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  return $data;
}

/**
 * Implements hook_views_data_alter().
 */
function payment_views_data_alter(&$data) {
  $data['payment']['description']['field'] = array(
    'handler' => 'PaymentViewsHandlerFieldTranslatableString',
    'string_arguments_field' => 'description_arguments',
  );
  $data['payment']['operations'] = array(
    'title' => t('Operations'),
    'real field' => 'pid',
    'field' => array(
      'handler' => 'PaymentViewsHandlerFieldPaymentOperations',
    ),
  );
  $data['payment']['pid']['relationship'] = array(
    'title' => t('Status items'),
    'base' => 'payment_status_item',
    'base field' => 'pid',
    'handler' => 'views_handler_relationship',
    'label' => t('Payment status item'),
  );
  $data['payment']['pid_line_item'] = array(
    'title' => t('Line items'),
    'real field' => 'pid',
    'relationship' => array(
      'title' => t('Line items'),
      'base' => 'payment_line_item',
      'base field' => 'pid',
      'handler' => 'views_handler_relationship',
      'label' => t('Line items'),
    ),
  );
  $data['payment']['pmid']['relationship'] = array(
    'title' => t('Payment method'),
    'base' => 'payment_method',
    'base field' => 'pmid',
    'handler' => 'views_handler_relationship',
    'label' => t('Payment method'),
  );
  $data['payment']['uid']['relationship'] = array(
    'title' => t('User'),
    'base' => 'users',
    'handler' => 'views_handler_relationship',
    'label' => t('User'),
  );

  if (payment_currency_version() == 2) {
    $data['payment_line_item']['amount']['field']['handler'] = 'CurrencyAmountViewsHandlerField';
    $data['payment_line_item']['amount_total']['field']['handler'] = 'CurrencyAmountViewsHandlerField';
  }

  // Payment method.
  $data['payment_method']['controller_description'] = array(
    'title' => t('Payment method type description'),
    'real field' => 'controller_class_name',
    'field' => array(
      'handler' => 'PaymentViewsHandlerFieldPaymentMethodControllerDescription',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
  $data['payment_method']['enabled_machine_name'] = array(
    'title' => t('Enabled (machine name)'),
    'real field' => 'enabled',
    'field' => array(
      'handler' => 'PaymentViewsHandlerFieldPaymentMethodEnabledMachineName',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
  $data['payment_method']['controller_title'] = array(
    'title' => t('Payment method type title'),
    'real field' => 'controller_class_name',
    'field' => array(
      'handler' => 'PaymentViewsHandlerFieldPaymentMethodControllerTitle',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'PaymentViewsHandlerFilterPaymentMethodControllerTitle',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
  $data['payment_method']['operations'] = array(
    'title' => t('Operations'),
    'real field' => 'pmid',
    'field' => array(
      'handler' => 'PaymentViewsHandlerFieldPaymentMethodOperations',
    ),
  );
  $data['payment_method']['uid']['relationship'] = array(
    'title' => t('User'),
    'base' => 'users',
    'handler' => 'views_handler_relationship',
    'label' => t('User'),
  );

  // Payment status items. This is not an alteration, but implementing
  // hook_views_data() prevents Entity API from exposing our entities to Views
  // automatically.
  $data['payment_status_item'] = array(
    'table' => array(
      'base' => array(
        'field' => 'psiid',
        'title' => t('Payment status item'),
      ),
      'group' => t('Payment status item'),
    ),
    // Field configuration.
    'created' => array(
      'title' => t('Creation date and time'),
      'field' => array(
        'handler' => 'views_handler_field_date',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort_date',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_date',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_date',
      ),
    ),
    'status' => array(
      'title' => t('Status code'),
      'field' => array(
        'handler' => 'views_handler_field',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'PaymentViewsHandlerFilterPaymentStatusItemStatus',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
    ),
    'pid' => array(
      'title' => t('Payment ID'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
      'relationship' => array(
        'title' => t('Payment'),
        'base' => 'payment',
        'handler' => 'views_handler_relationship',
        'label' => t('Payment'),
      ),
    ),
    'psiid' => array(
      'title' => t('Payment status item ID'),
      'field' => array(
        'handler' => 'views_handler_field_numeric',
        'click sortable' => TRUE,
      ),
      'sort' => array(
        'handler' => 'views_handler_sort',
      ),
      'filter' => array(
        'handler' => 'views_handler_filter_numeric',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
    ),
    'title' => array(
      'title' => t('Status title'),
      'real field' => 'status',
      'field' => array(
        'handler' => 'PaymentViewsHandlerFieldPaymentStatusTitle',
      ),
    ),
  );

  // {users}
  $data['users']['uid_payment'] = array(
    'title' => t('Payments'),
    'real field' => 'uid',
    'relationship' => array(
      'title' => t('Payments'),
      'base' => 'payment',
      'base field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('Payments'),
    ),
  );
}

/**
 * Implements hook_views_plugins().
 */
function payment_views_plugins() {
  $plugins = array();
  $plugins['access']['payment_own_payment_overview'] = array(
    'title' => t('People with the <em>View any payment</em> permission and people who own the payments'),
    'handler' => 'PaymentViewsHandlerAccessOwnPaymentOverview',
  );

  return $plugins;
}
