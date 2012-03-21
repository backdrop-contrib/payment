<?php

/**
 * @file Views integration.
 */

/**
 * Implements hook_views_data_alter().
 */
function payment_views_data_alter(&$data) {
  // Payment.
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
  $data['payment']['pid_amount'] = array(
    'title' => t('Payment amounts'),
    'real field' => 'pid',
    'relationship' => array(
      'title' => t('Payment amounts'),
      'base' => 'payment_amount',
      'base field' => 'pid',
      'handler' => 'views_handler_relationship',
      'label' => t('Payment amounts'),
    ),
  );
  $data['payment']['pmid']['relationship'] = array(
    'title' => t('Payment method'),
    'base' => 'payment_method',
    'handler' => 'views_handler_relationship',
    'label' => t('Payment method'),
  );
  $data['payment']['uid']['relationship'] = array(
    'title' => t('User'),
    'base' => 'users',
    'handler' => 'views_handler_relationship',
    'label' => t('User'),
  );

  // Payment amounts. This is not an alteration, but implementing
  // hook_views_data() prevents Entity API from exposing our entities to Views
  // automatically. See http://drupal.org/node/1377364.
  $data['payment_amount'] = array(
    'table' => array(
      'base' => array(
        'title' => t('Payment amount'),
      ),
      'group' => t('Payment amount'),
    ),
    // Field configuration.
    'amount' => array(
      'title' => t('Amount'),
      'field' => array(
        'handler' => 'views_handler_field',
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
        'handler' => 'PaymentViewsHandlerFieldPaymentAmountDescription',
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
        'handler' => 'views_handler_filter_numeric',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_numeric',
      ),
    ),
    'pid' => array(
      'title' => t('Payment ID'),
      'field' => array(
        'handler' => 'views_handler_field',
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
  );

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
      'handler' => 'views_handler_filter_string',
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
        'handler' => 'views_handler_filter_string',
      ),
      'argument' => array(
        'handler' => 'views_handler_argument_string',
      ),
    ),
    'pid' => array(
      'title' => t('Payment ID'),
      'field' => array(
        'handler' => 'views_handler_field',
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
        'handler' => 'views_handler_field',
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
}