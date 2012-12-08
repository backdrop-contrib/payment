<?php

/**
 * @file
 * Implementations of Xtools hooks.
 */

/**
 * Implements hook_xtools_blueprint_info().
 */
function payment_xtools_blueprint_info() {
  // hook_payment_line_item_info() implementation return value.
  $blueprints['hook_payment_line_item_info'] = new XtoolsBlueprintArray(array(), array(
    new XtoolsBlueprintPlaceholder('PaymentLineItemInfo'),
  ), 'integer');

  // hook_payment_method_controller_info() implementation return value.
  $blueprints['hook_payment_method_controller_info'] = new XtoolsBlueprintArray(array(), array(
    new XtoolsBlueprintString,
  ));

  // hook_payment_status_info() implementation return value.
  $blueprints['hook_payment_status_info'] = new XtoolsBlueprintArray(array(), array(
    new XtoolsBlueprintPlaceholder('PaymentStatusInfo'),
  ), 'integer');

  // Payment
  $blueprints['Payment'] = new XtoolsBlueprintObject('Payment', array(
    'context' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString(0, 255), TRUE),
    'context_data' => new XtoolsBlueprintChildElement(new XtoolsBlueprintArray(), TRUE),
    'currency_code' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString(3, 3), TRUE),
    'description' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString(0, 255), TRUE),
    'description_arguments' => new XtoolsBlueprintChildElement(new XtoolsBlueprintArray(array(), array(), 'string'), TRUE),
    'finish_callback' => new XtoolsBlueprintChildElement(new XtoolsBlueprintCallable(), TRUE),
    'line_items' => new XtoolsBlueprintChildElement(new XtoolsBlueprintArray(array(), array(new XtoolsBlueprintPlaceholder('PaymentLineItem')), 'string'), TRUE),
    'method' => new XtoolsBlueprintChildElement(new XtoolsBlueprintPlaceholder('PaymentMethod'), TRUE),
    'method_data' => new XtoolsBlueprintChildElement(new XtoolsBlueprintArray(), TRUE),
    'pid' => new XtoolsBlueprintChildElement(new XtoolsBlueprintInteger(0), TRUE),
    'line_items' => new XtoolsBlueprintChildElement(new XtoolsBlueprintArray(array(), array(new XtoolsBlueprintPlaceholder('PaymentLineItem'))), TRUE),
    'uid' => new XtoolsBlueprintChildElement(new XtoolsBlueprintOr(array(new XtoolsBlueprintNULL(), new XtoolsBlueprintInteger(0))), TRUE),
  ));

  // PaymentLineItem
  $blueprints['PaymentLineItem'] = new XtoolsBlueprintObject('PaymentLineItem', array(
    'amount' => new XtoolsBlueprintChildElement(new XtoolsBlueprintFloat(0), TRUE),
    'description' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString(0, 255), TRUE),
    'description_arguments' => new XtoolsBlueprintChildElement(new XtoolsBlueprintArray(array(), array(), 'string'), TRUE),
    'name' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString, TRUE),
    'tax_rate' => new XtoolsBlueprintChildElement(new XtoolsBlueprintFloat(0), TRUE),
    'quantity' => new XtoolsBlueprintChildElement(new XtoolsBlueprintInteger(0), TRUE),
  ));

  // PaymentLineItemInfo
  $blueprints['PaymentLineItemInfo'] = new XtoolsBlueprintObject('PaymentLineItemInfo', array(
    'callback' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString(0, 255), TRUE),
    'name' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString(0, 255), TRUE),
    'title' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString(0, 255), TRUE),
  ));

  // PaymentMethod
  $blueprints['PaymentMethod'] = new XtoolsBlueprintObject('PaymentMethod', array(
    'controller' => new XtoolsBlueprintChildElement(new XtoolsBlueprintObject('PaymentMethodController'), TRUE),
    'controller_data' => new XtoolsBlueprintChildElement(new XtoolsBlueprintArray(), TRUE),
    'enabled' => new XtoolsBlueprintChildElement(new XtoolsBlueprintBoolean, TRUE),
    'name' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString(0, 255), TRUE),
    'module' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString(0, 255), TRUE),
    'pmid' => new XtoolsBlueprintChildElement(new XtoolsBlueprintInteger(0), TRUE),
    'status' => new XtoolsBlueprintChildElement(new XtoolsBlueprintInteger(0), TRUE),
    'title_specific' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString(0, 255), TRUE),
    'title_generic' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString(0, 255), TRUE),
    'uid' => new XtoolsBlueprintChildElement(new XtoolsBlueprintOr(array(new XtoolsBlueprintNULL(), new XtoolsBlueprintInteger(0))), TRUE),
  ));

  // PaymentStatusInfo
  $blueprints['PaymentStatusInfo'] = new XtoolsBlueprintObject('PaymentStatusInfo', array(
    'description' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString),
    'parent' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString),
    'status' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString, TRUE),
    'title' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString, TRUE),
  ));

  // PaymentStatusItem
  $blueprints['PaymentStatusItem'] = new XtoolsBlueprintObject('PaymentStatusItem', array(
    'created' => new XtoolsBlueprintChildElement(new XtoolsBlueprintInteger, TRUE),
    'pid' => new XtoolsBlueprintChildElement(new XtoolsBlueprintInteger, TRUE),
    'psiid' => new XtoolsBlueprintChildElement(new XtoolsBlueprintInteger, TRUE),
    'status' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString, TRUE),
  ));

  return $blueprints;
}

/**
 * Implements hook_xtools_callable_type_info().
 */
function payment_xtools_callable_type_info() {
  return array(
    new XtoolsCallableTypeHook('payment_form_alter', array(
      'signature' => new XtoolsSignaturePlaceholder('hook_payment_form_alter'),
    )),
    new XtoolsCallableTypeHook('payment_line_item_info', array(
      'blueprint' => new XtoolsBlueprintPlaceholder('hook_payment_line_item_info'),
    )),
    new XtoolsCallableTypeHook('payment_line_item_info_alter', array(
      'signature' => new XtoolsSignaturePlaceholder('hook_payment_line_item_info_alter'),
    )),
    new XtoolsCallableTypeHook('payment_method_controller_info', array(
      'blueprint' => new XtoolsBlueprintPlaceholder('hook_payment_method_controller_info'),
    )),
    new XtoolsCallableTypeHook('payment_method_controller_info_alter', array(
      'signature' => new XtoolsSignaturePlaceholder('hook_payment_method_controller_info_alter'),
    )),
    new XtoolsCallableTypeHook('payment_pre_execute', array(
      'signature' => new XtoolsSignaturePlaceholder('hook_payment_pre_execute'),
    )),
    new XtoolsCallableTypeHook('payment_status_change', array(
      'signature' => new XtoolsSignaturePlaceholder('hook_payment_status_change'),
    )),
    new XtoolsCallableTypeHook('payment_status_info', array(
      'blueprint' => new XtoolsBlueprintPlaceholder('hook_payment_status_info'),
    )),
    new XtoolsCallableTypeHook('payment_status_info_alter', array(
      'signature' => new XtoolsSignaturePlaceholder('hook_payment_status_info_alter'),
    )),
    new XtoolsCallableTypeHook('payment_validate', array(
      'signature' => new XtoolsSignaturePlaceholder('hook_payment_validate'),
    )),
  );
}

/**
 * Implements hook_xtools_signature_info().
 */
function payment_xtools_signature_info() {
  $signatures['hook_payment_form_alter'] = new XtoolsSignature(array(
    new XtoolsSignatureParameter(array(
      'name' => 'elements',
      'reference' => TRUE,
      'type' => 'array',
    )),
    new XtoolsSignatureParameter(array(
      'name' => 'form_state',
      'reference' => TRUE,
      'type' => 'array',
    )),
    new XtoolsSignatureParameter(array(
      'name' => 'submit',
      'reference' => TRUE,
      'type' => 'array',
    )),
  ));
  $signatures['hook_payment_line_item_info_alter'] = new XtoolsSignature(array(
    new XtoolsSignatureParameter(array(
      'name' => 'line_items_info',
      'reference' => TRUE,
      'type' => 'array',
    )),
  ));
  $signatures['hook_payment_method_controller_info_alter'] = new XtoolsSignature(array(
    new XtoolsSignatureParameter(array(
      'name' => 'controllers_info',
      'reference' => TRUE,
      'type' => 'array',
    )),
  ));
  $signatures['hook_payment_pre_execute'] = new XtoolsSignature(array(
    new XtoolsSignatureParameter(array(
      'name' => 'payment',
      'type' => 'Payment',
    )),
  ));
  $signatures['hook_payment_status_change'] = new XtoolsSignature(array(
    new XtoolsSignatureParameter(array(
      'name' => 'payment',
      'type' => 'Payment',
    )),
    new XtoolsSignatureParameter(array(
      'name' => 'previous_status_item',
      'type' => 'PaymentStatusItem',
    )),
  ));
  $signatures['hook_payment_status_info_alter'] = new XtoolsSignature(array(
    new XtoolsSignatureParameter(array(
      'name' => 'statuses_info',
      'reference' => TRUE,
      'type' => 'array',
    )),
  ));
  $signatures['hook_payment_validate'] = new XtoolsSignature(array(
    new XtoolsSignatureParameter(array(
      'name' => 'payment',
      'type' => 'Payment',
    )),
    new XtoolsSignatureParameter(array(
      'name' => 'payment_method',
      'type' => 'PaymentMethod',
    )),
    new XtoolsSignatureParameter(array(
      'name' => 'strict',
    )),
  ));

  return $signatures;
}