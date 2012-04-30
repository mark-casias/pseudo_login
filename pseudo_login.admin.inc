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
  if (empty($form_state['form_info'])) {
    $form_state['form_info'] = pseudo_login_form_info();
  }
  $fields = pseudo_login_fields_info();
  
  $field_options = array( '' => t('- None -'));
  foreach ($fields as $field_name => $field) {
    $field_options[$field_name] = $field['type'] . ': ' . $field['title'];
  }
  
  $form['pseudo_login_tabs'] = array(
    '#type' => 'vertical_tabs',
  );
  
  $i = 0;
  foreach ($form_state['form_info'] as $form_id => $info) {
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
      'source' => array(
        '#type' => 'textfield',
        '#title' => t('Source Code (for new users)'),
        '#default_value' => $info['source'],
      ),
      'fields' => array(
        '#attributes' => array(
          'id' => "fields_$key",
        ),
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
    $form['pseudo_login_tabs'][$key]['add'] = array(
      '#type' => 'fieldset',
      '#title' => t('Add a new field'),
      '#tree' => TRUE,
      '#element_validate' => array('pseudo_login_config_form_validate_add'),
      
      'name' => array(
        '#type' => 'textfield',
        '#title' => t('New field name'),
        '#description' => t('The internal name of the field within the form.'),
      ),
      'button' => array(
        '#type' => 'submit',
        '#name' => "add-$key",
        '#value' => t('Add'),
        '#form_id' => $form_id,
        '#key' => $key,
        '#submit' => array('pseudo_login_config_form_add'),
        '#ajax' => array(
          'callback' => 'pseudo_login_config_form_ajax',
          'wrapper' => "fields_$key",
          'progress' => array('type' => 'throbber'),
        ),
      ),
    );
  }
  return system_settings_form($form);
}

function pseudo_login_config_form_validate_add($element, &$form_state) {
  if (empty($form_state['triggering_element']['#key'])) {
    form_set_value($element, NULL, $form_state);
  }
}

function pseudo_login_config_form_add($form, &$form_state) {
  $form_id = $form_state['triggering_element']['#form_id'];
  $key = $form_state['triggering_element']['#key'];
  
  // Get the submitted name and add it to the info list.
  if (!empty($form_state['values'][$key]['add']['name'])) {
    $name = $form_state['values'][$key]['add']['name'];
    $form_state['form_info'][$form_id]['fields'][$name] = '';
  }
  $form_state['rebuild'] = TRUE;
}

function pseudo_login_config_form_ajax($form, &$form_state) {
  $key = $form_state['triggering_element']['#key'];
  return $form['pseudo_login_tabs'][$key]['fields'];
}