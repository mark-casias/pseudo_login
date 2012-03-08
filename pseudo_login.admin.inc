<?php
/**
 * @file 
 * 
 * Copyright 2011 New Signature
 * http://www.newsignature.com
 *
 * @author Andrew Marcus
 * @since Aug 16, 2011
 */

function pseudo_login_config_form($form, &$form_state) {
  $form['pseudo_login_token'] = array(
    '#type' => 'textfield',
    '#title' => t('Token Key'),
    '#description' => t('This is the key that the login url will look for'),
    '#default_value' => variable_get('pseudo_login_token', '_u'),
  );
  
  // Lookup all fields and extra fields attached to the user object.
  $forms = pseudo_login_form_info();
  $fields = pseudo_login_fields_info();
  
  $field_options = array('' => t('Ignore'));
  foreach ($fields as $field_name => $field) {
    $field_options[$field_name] = $field['type'] . ': ' . $field['title'];
  }
  
  $form['pseudo_login_tabs'] = array(
    '#type' => 'vertical_tabs',
  );
  
  $i = 0;
  foreach ($forms as $form_id => $info) {
    $key = "pseudo_login_form_{$form_id}";
    
    $form['pseudo_login_tabs'][$key] = array(
      '#type' => 'fieldset',
      '#title' => $info['title'],
      '#tree' => TRUE,
      '#collapsible' => TRUE,
    
      'fieldset_title' => array(
        '#type' => 'textfield',
        '#title' => t('Fieldset Title'),
        '#default_value' => $info['fieldset_title'],
      ),
      'fields' => array(
        '#type' => 'fieldset',
        '#title' => t('Field Mapping'),
        '#collapsible' => FALSE,
      ),
    );
    
    
    $active_tab = variable_get('pseudo_login_tabs__active_tab', '');
    if ($active_tab == 'edit-' . str_replace('_', '-', $key)) {
      $form['tabs']['#default_tab'] = $key;
    }
    
    foreach ($info['fields'] as $field => $default) {
      $form['pseudo_login_tabs'][$key]['fields'][$field] = array(
        '#type' => 'select',
        '#title' => $field,
        '#options' => $field_options,
        '#default_value' => $default,
      );
    }
  }
  return system_settings_form($form);
}