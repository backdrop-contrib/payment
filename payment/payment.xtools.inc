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
    new XtoolsBlueprintObject('PaymentLineItemInfo'),
  ), 'integer');

  // hook_payment_method_controller_info() implementation return value.
  $blueprints['hook_payment_method_controller_info'] = new XtoolsBlueprintArray(array(
    new XtoolsBlueprintString,
  ));

  // hook_payment_status_info() implementation return value.
  $blueprints['hook_payment_status_info'] = new XtoolsBlueprintArray(array(
    new XtoolsBlueprintObject('PaymentStatusInfo'),
  ), 'integer');

  return $blueprints;
}