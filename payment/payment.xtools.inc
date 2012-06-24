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
  $blueprints['hook_payment_line_item_info'] = new XtoolsBlueprintArray(array(
    new XtoolsBlueprintInfo('PaymentLineItemInfo'),
  ), 'integer');

  // hook_payment_method_controller_info() implementation return value.
  $blueprints['hook_payment_method_controller_info'] = new XtoolsBlueprintArray(array(
    new XtoolsBlueprintString,
  ));

  // hook_payment_status_info() implementation return value.
  $blueprints['hook_payment_status_info'] = new XtoolsBlueprintArray(array(
    new XtoolsBlueprintInfo('PaymentStatusInfo'),
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
    new XtoolsCallableTypeHook('payment_line_item_info', array(
      'blueprint' => new XtoolsBlueprintInfo('hook_payment_line_item_info'),
    )),
    new XtoolsCallableTypeHook('payment_method_controller_info', array(
      'blueprint' => new XtoolsBlueprintInfo('hook_payment_method_controller_info'),
    )),
    new XtoolsCallableTypeHook('payment_status_info', array(
      'blueprint' => new XtoolsBlueprintInfo('hook_payment_status_info'),
    )),
  );
}