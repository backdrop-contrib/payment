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
  $actions['payment_rules_action_payment_amount_delete'] = array(
    'label' => t('Delete a payment amount'),
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
  $actions['payment_rules_action_payment_amount_set_fixed'] = array(
    'label' => t('Set a fixed payment amount'),
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
      ),
      'amount' => array(
        'type' => 'decimal',
        'label' => t('Amount'),
      ),
    ),
  );
  $actions['payment_rules_action_payment_amount_set_calculated'] = array(
    'label' => t('Set a calculated payment amount'),
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
        'optional' => TRUE,
      ),
      'source_name' => array(
        'type' => 'text',
        'label' => t("Source amount"),
        'options list' => 'payment_rules_options_list_payment_amount_name',
      ),
      'calculation' => array(
        'type' => 'text',
        'label' => t("Calculation"),
        'options list' => 'payment_rules_options_list_calculations',
      ),
      'factor' => array(
        'type' => 'decimal',
        'label' => 'Amount',
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
 * Implements Rules action callback: set a fixed payment amount.
 *
 * @param $payment Payment
 * @param $amount integer
 *   The amount to add to the payment amount.
 */
function payment_rules_action_payment_amount_set_fixed(Payment $payment, $name, $description, $amount) {
  $payment->setAmount($name, $amount, $description);
}

/**
 * Implements Rules action callback: set a calculated payment amount.
 *
 * @see PaymentAmountItem
 *
 * @param $payment Payment
 * @param $name string
 *   The machine name of the new amount.
 * @param $description string
 *   The amount's human-readable description.
 * @param $source_name string
 *   The machine name of the payment amount to use for the calculation.
 * @param $calculation string
 *   One of the keys returned by payment_rules_options_list_calculations().
 * @param $factor float
 *   The factor to use for the calculation.
 */
function payment_rules_action_payment_amount_set_calculated($payment, $name, $description, $source_name, $calculation, $factor) {
  if ($source_amounts = payment_amount_extract($source_name, $payment)) {
    $source_amount = $payment->totalAmount($source_amounts);
    switch ($calculation) {
      case 'add':
        $amount = $source_amount + $factor;
        break;
      case 'subtract':
        $amount = $source_amount - $factor;
        break;
      case 'multiply':
        $amount = $source_amount * $factor;
        break;
      case 'divide':
        $amount = $source_amount / $factor;
        break;
    }
    $payment->setAmount($name, $amount, $description);
  }
}

/**
 * Implements Rules options list callback for
 * payment_rules_action_payment_amount_set_calculated().
 */
function payment_rules_options_list_payment_amount_name() {
  $options = array();
  foreach (payment_amounts_info() as $amount_info) {
    $options[$amount_info->name] = $amount_info->title;
  }

  return $options;
}

/**
 * Implements Rules options list callback for
 * payment_rules_action_payment_amount_set_calculated().
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
 * Implements Rules action callback: delete a payment amount.
 *
 * @param $payment Payment
 * @param $name string
 *   The machine name of the payment amount to delete.
 */
function payment_rules_action_payment_amount_delete(Payment $payment, $name) {
  unset($payment->amounts[$name]);
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