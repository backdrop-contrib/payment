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

  // PaymentLineItemInfo
  $blueprints['PaymentLineItemInfo'] = new XtoolsBlueprintObject('PaymentLineItemInfo', array(
    'callback' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString, TRUE),
    'name' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString, TRUE),
    'title' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString, TRUE),
  ));

  // PaymentStatusInfo
  $blueprints['PaymentStatusInfo'] = new XtoolsBlueprintObject('PaymentStatusInfo', array(
    'description' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString),
    'parent' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString),
    'status' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString, TRUE),
    'title' => new XtoolsBlueprintChildElement(new XtoolsBlueprintString, TRUE),
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