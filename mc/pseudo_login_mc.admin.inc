<?php
function pseudo_login_mc_config_form($form, &$form_state) {
  $messages = pseudo_login_mc_messages();
  
  $form['pseudo_login_mc_messages'] = array(
    '#type' => 'fieldset',
    '#title' => t('Status Messages'),
    '#description' => t('The message text to be displayed when a user signs up.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
    
    'subscribed' => array(
      '#type' => 'textarea',
      '#rows' => '2',
      '#title' => t('User is already subscribed.'),
      '#default_value' => $messages['subscribed'],
    ),
    'new' => array(
      '#type' => 'textarea',
      '#rows' => '2',
      '#title' => t('User has been subscribed to a regular list.'),
      '#default_value' => $messages['new'],
    ),
    'double_opt_in' => array(
      '#type' => 'textarea',
      '#rows' => '2',
      '#title' => t('User has been subscribed to a double-opt-in list.'),
      '#default_value' => $messages['double_opt_in'],
    ),
  );
  return system_settings_form($form);
}