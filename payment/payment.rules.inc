<?php

/**
 * @file
 * Rules integration.
 */

/**
 * Implements hook_rules_event_info().
 */
function payment_rules_event_info() {
  $event['payment_status_change'] = array(
    'label' => t("After changing a payment's status"),
    'group' => t('Payment'),
    'access callback' => 'payment_rules_access',
    'variables' => array(
      'payment' => array(
        'type' => 'payment',
        'label' => t('Payment'),
      ),
      'payment_old_status' => array(
        'type' => 'text',
        'label' => t("The payment's old status"),
      ),
    ),
  );
  $event['payment_pre_execute'] = array(
    'label' => t('Before executing a payment'),
    'group' => t('Payment'),
    'access callback' => 'payment_rules_access',
    'variables' => array(
      'payment' => array(
        'type' => 'payment',
        'label' => t('Payment'),
      ),
    ),
  );

  return $event;
}

/**
 * Implements hook_rules_action_info().
 */
function payment_rules_action_info() {
  $actions['payment_rules_action_payment_set_status'] = array(
    'label' => t('Set payment status'),
    'access callback' => 'payment_rules_access',
    'parameter' => array(
      'payment' => array(
        'type' => 'payment',
        'label' => t('Label'),
      ),
      'status' => array(
        'type' => 'text',
        'label' => t('Status'),
        'options list' => 'payment_rules_options_list_payment_set_status',
      ),
    ),
    'group' => t('Payment'),
  );
  $actions['payment_rules_action_payment_line_item_delete'] = array(
    'label' => t('Delete a payment line item'),
    'access callback' => 'payment_rules_access',
    'group' => t('Payment'),
    'parameter' => array(
      'payment' => array(
        'type' => 'payment',
        'label' => t('Payment'),
      ),
      'name' => array(
        'type' => 'text',
        'label' => t('Machine name'),
      ),
    ),
  );
  $description = array(t('If the evaluated amount does not return a whole or fractional number, this action will do nothing.'));
  $description[] = module_exists('ctools') ? t('You can use mathematical expressions.') : t('Enable <a href="http://drupal.org/project/ctools">Chaos tool suite</a> in order to use mathematical expressions.');
  $description = implode(' ', $description);
  $actions['payment_rules_action_payment_line_item_add'] = array(
    'label' => t('Add a line item'),
    'access callback' => 'payment_rules_access',
    'group' => t('Payment'),
    'parameter' => array(
      'payment' => array(
        'type' => 'payment',
        'label' => t('Payment'),
      ),
      'name' => array(
        'type' => 'text',
        'label' => t('Machine name'),
      ),
      'description' => array(
        'type' => 'text',
        'label' => t('Description'),
        'translatable' => TRUE,
      ),
      'tax_rate' => array(
        'type' => 'decimal',
        'label' => t('Tax rate'),
        'description' => t('Enter a (fractional) number between 0 and 1.'),
        'default value' => 0,
      ),
      'quantity' => array(
        'type' => 'integer',
        'label' => t('Quantity'),
        'default value' => 1,
      ),
      'amount' => array(
        'type' => 'text',
        'label' => t('Amount'),
        'description' => $description,
      ),
    ),
  );

  return $actions;
}

/**
 * Implements hook_rules_condition_info().
 */
function payment_rules_condition_info() {
  $conditions['payment_rules_condition_payment_status_is'] = array(
    'label' => t('Payment status is'),
    'access callback' => 'payment_rules_access',
    'parameter' => array(
      'payment' => array(
        'label' => t('Payment'),
        'type' => 'payment',
        'restriction' => 'selector',
      ),
      'payment_statuses' => array(
        'type' => 'list<text>',
        'label' => t('Payment status'),
        'options list' => 'payment_rules_options_list_payment_statuses',
        'restriction' => 'input',
      ),
    ),
    'group' => t('Payment'),
  );
  $conditions['payment_rules_condition_payment_status_has_ancestor'] = array(
    'label' => t('Payment status is derived from'),
    'access callback' => 'payment_rules_access',
    'parameter' => array(
      'payment' => array(
        'label' => t('Payment'),
        'type' => 'payment',
        'restriction' => 'selector',
      ),
      'payment_statuses' => array(
        'type' => 'list<text>',
        'label' => t('Payment status is or is derived from'),
        'options list' => 'payment_rules_options_list_payment_status_parents',
        'restriction' => 'input',
      ),
    ),
    'group' => t('Payment'),
  );
  $conditions['payment_rules_condition_payment_uses_payment_method'] = array(
    'label' => t('Payment method is'),
    'access callback' => 'payment_rules_access',
    'parameter' => array(
      'payment' => array(
        'label' => t('Payment'),
        'type' => 'payment',
        'restriction' => 'selector',
      ),
      'pmids' => array(
        'type' => 'list<text>',
        'label' => t('Payment methods'),
        'options list' => 'payment_rules_options_list_payment_uses_payment_method',
        'restriction' => 'input',
      ),
    ),
    'group' => t('Payment'),
  );
  $conditions['payment_rules_condition_payment_uses_payment_method_type'] = array(
    'label' => t('Payment method type is'),
    'access callback' => 'payment_rules_access',
    'parameter' => array(
      'payment' => array(
        'label' => t('Payment'),
        'type' => 'payment',
        'restriction' => 'selector',
      ),
      'payment_method_controller_class_names' => array(
        'type' => 'list<text>',
        'label' => t('Payment method types'),
        'options list' => 'payment_rules_options_list_payment_uses_payment_method_type',
        'restriction' => 'input',
      ),
    ),
    'group' => t('Payment'),
  );
  $conditions['payment_rules_condition_payment_method_controller_supports_currency'] = array(
    'label' => t('Payment method type supports currency'),
    'access callback' => 'payment_rules_access',
    'parameter' => array(
      'payment' => array(
        'label' => t('Payment'),
        'type' => 'payment',
        'restriction' => 'selector',
      ),
      'currency' => array(
        'type' => 'text',
        'label' => t('Currency code'),
      ),
    ),
    'group' => t('Payment'),
  );

  return $conditions;
}

/**
 * Implements Rules condition callback: check whether a payment's status
 * is one of the selected statuses.
 *
 * @param $payment Payment
 *   The payment to check the condition for.
 * @param $payment_statuses array
 *   An array with payment statuses.
 *
 * @return boolean
 */
function payment_rules_condition_payment_status_is(Payment $payment, array $payment_statuses) {
  return in_array($payment->status, $payment_statuses);
}

/**
 * Implements Rules options list: list all payment statuses.
 */
function payment_rules_options_list_payment_statuses() {
  $options = array();
  foreach (payment_statuses_info() as $status_info) {
    $options[$status_info->status] = $status_info->title;
  }
  asort($options);

  return $options;
}

/**
 * Implements Rules condition callback: check whether a payment's status
 * is a child of any of the selected statuses.
 *
 * @param $payment Payment
 *   The payment to check the condition for.
 * @param $payment_statuses array
 *   An array with payment statuses.
 *
 * @return boolean
 */
function payment_rules_condition_payment_status_has_ancestor(Payment $payment, array $payment_statuses) {
  foreach ($payment_statuses as $payment_status) {
    if (payment_status_has_ancestor($payment->status, $payment_status)) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Implements Rules options list: list all parent statuses.
 */
function payment_rules_options_list_payment_status_parents() {
  $statuses_info = payment_statuses_info();
  $parents = array();
  foreach ($statuses_info as $status_info) {
    $parents = array_merge($parents, $status_info->parents);
  }
  $options = array();
  foreach (array_unique($parents) as $status) {
    $options[$status] = $statuses_info[$status]->title;
  }
  asort($options);

  return $options;
}

/**
 * Implements Rules action callback for payment_rules_payment_set_status.
 */
function payment_rules_action_payment_set_status(Payment $payment, $status_data) {
  $payment->setStatus(reset($status_data));
}

/**
 * Implements Rules action callback: add a line item.
 *
 * @param Payment $payment
 * @param string $name
 *   The line item's unique machine name.
 * @param string $description
 * @param float $tax_rate
 * The tax rate that applies to the amount. Should be a float between 0 and 1.
 * @param integer $quantity
 * @param integer|string $amount
 *   The amount of money, either as an integer or string that contains
 *   placeholders or a Ctools math expression.
 */
function payment_rules_action_payment_line_item_add(Payment $payment, $name, $description, $tax_rate, $quantity, $amount) {
  if (!is_numeric($amount)) {
    // Replace placeholders.
    $amount = token_replace($amount, array(
      'payment' => $payment,
    ));

    // Evaluate Ctools math expression.
    if (module_exists('ctools')) {
      $math = new ctools_math_expr;
      $amount = $math->evaluate($amount);
    }
  }
  if (is_numeric($amount)) {
    $payment->setLineItem(new PaymentLineItem(array(
      'name' => $name,
      'amount' => $amount,
      'description'=> $description,
      'tax_rate' => $tax_rate,
      'quantity' => $quantity,
    )));
  }
}

/**
 * Implements Rules options list callback for
 * payment_rules_action_payment_line_item_set_calculated().
 */
function payment_rules_options_list_payment_line_item_name() {
  $options = array();
  foreach (payment_line_items_info() as $line_item_info) {
    $options[$line_item_info->name] = $line_item_info->title;
  }

  return $options;
}

/**
 * Implements Rules options list callback for
 * payment_rules_action_payment_line_item_set_calculated().
 */
function payment_rules_options_list_calculations() {
  return array(
    'add' => t('Add'),
    'subtract' => t('Subtract'),
    'multiply' => t('Multiply by'),
    'divide' => t('Divide by'),
  );
}

/**
 * Implements Rules action callback: delete a line item.
 *
 * @param $payment Payment
 * @param $name string
 *   The machine name of the line item to delete.
 */
function payment_rules_action_payment_line_item_delete(Payment $payment, $name) {
  unset($payment->line_items[$name]);
}

/**
 * Implements Rules options list callback for
 * payment_rules_payment_status_change.
 */
function payment_rules_options_list_payment_set_status() {
  $options = array();
  foreach (payment_statuses_info() as $status_info) {
    $options[$status_info->status] = $status_info->title;
  }
  asort($options);

  return $options;
}

/**
 * Implements Rules options list callback for
 * payment_rules_condition_payment_uses_payment_method.
 */
function payment_rules_options_list_payment_uses_payment_method() {
  $options = array();
  foreach (entity_load('payment_method') as $payment_method) {
    $options[$payment_method->pmid] = $payment_method->title_specific;
  }

  return $options;
}

/**
 * Implements Rules condition callback: check whether a payment uses one of the
 * selected payment methods.
 *
 * @param $payment Payment
 *   The payment to check the condition for.
 * @param $pmids array
 *   An array with payment method IDs.
 *
 * @return boolean
 */
function payment_rules_condition_payment_uses_payment_method(Payment $payment, array $pmids) {
  return in_array($payment->method->pmid, $pmids);
}

/**
 * Implements Rules options list callback for
 * payment_rules_condition_payment_uses_payment_method_type.
 */
function payment_rules_options_list_payment_uses_payment_method_type() {
  $options = array();
  foreach (payment_method_controller_load_multiple() as $payment_method_controller) {
    $options[$payment_method_controller->name] = $payment_method_controller->title;
  }

  return $options;
}

/**
 * Implements Rules condition callback: check whether a payment uses one of the
 * selected payment methods types.
 *
 * @param $payment Payment
 *   The payment to check the condition for.
 * @param $payment_method_controller_class_names array
 *   An array with class names of payment method controllers.
 *
 * @return boolean
 */
function payment_rules_condition_payment_uses_payment_method_type(Payment $payment, array $payment_method_controller_class_names) {
  return in_array($payment->method->controller->name, $payment_method_controller_class_names);
}

/**
 * Implements Rules condition callback: check whether a payment method
 * controller supports a currency.
 *
 * @param $payment Payment
 *   The payment to check the condition for.
 * @param $currency string
 *   The ISO 4217 currency code to check for.
 *
 * @return boolean
 */
function payment_rules_condition_payment_method_controller_supports_currency(Payment $payment, $currency) {
  return empty($payment->method->controller->currencies) ? TRUE : in_array($currency, $payment->method->controller->currencies);
}