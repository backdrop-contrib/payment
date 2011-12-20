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
  $data['payment']['pmid']['relationship'] = array(
    'base' => 'payment_method',
    'field' => 'pmid',
    'handler' => 'views_handler_relationship',
    'label' => t('Payment method'),
  );
  $data['payment']['status_human_readable'] = array(
    'title' => t('Status (human-readable)'),
    'real field' => 'status',
    'field' => array(
      'handler' => 'PaymentViewsHandlerFieldPaymentStatus',
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
  $data['payment']['uid']['relationship'] = array(
    'base' => 'users',
    'field' => 'uid',
    'handler' => 'views_handler_relationship',
    'label' => t('User'),
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

  // Payment revision. This is not an alteration, but implementing
  // hook_views_data() prevents Entity API from exposing our entities to Views
  // automatically.
  $data['payment_revision'] = array(
    'table' => array(
      'group' => t('Payment revision'),
      'join' => array(
        'payment' => array(
          'left_field' => 'pid',
          'field' => 'pid',
        ),
      ),
    ),
    // Field configuration.
    'created_revision' => array(
      'title' => t('Created date and time'),
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
    'description' => array(
      'title' => t('Description'),
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
    'status' => array(
      'title' => t('Status'),
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
    'vid' => array(
      'title' => t('Revision ID'),
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
        'base' => 'payment',
        'field' => 'vid',
        'handler' => 'views_handler_relationship',
        'label' => t('Payment'),
      ),
    ),
  );
}