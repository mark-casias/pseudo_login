<?php
/**
 * @file 
 * 
 * Copyright 2012 New Signature
 * http://www.newsignature.com
 *
 * @author Andrew Marcus
 * @since Apr 24, 2012
 */

/**
 * Implements hook_pseudo_login_field_info().
 */
function pseudo_login_mc_pseudo_login_fields_info() {
  $info = array();
  
  $info['mc_list'] = array(
    'type' => t('MailChimp'),
    'title' => t('Optional Lists'),
    'base' => 'pseudo_login_mc_field',
    'show_lists' => TRUE,
    'show_selected' => FALSE,
    'select_first' => TRUE,
    'show_interests' => TRUE,
  );
  $info['mc_list_hidden'] = array(
    'type' => t('MailChimp'),
    'title' => t('Default list (hidden)'),
    'base' => 'pseudo_login_mc_field',
    'show_lists' => FALSE,
    'show_selected' => FALSE,
    'select_first' => TRUE,
    'show_interests' => FALSE,
  );
  
  return $info;
}

/**
 * Renders the form element for selecting MailChimp lists.
 * 
 * @param array $field The field definition from pseudo_login_pseudo_login_fields_info().
 * @param array $form_state The form state.
 * @param object $account The user account.
 * @param boolean $required If true, the field is required.
 */
function pseudo_login_mc_field_form($field, &$form_state, $account, $required = TRUE) {
  $element = array();
  
  $a = clone($account);
  
  // need to force feed the authenticated role to this account object so the
  // right newseletters are available
  $a->roles[DRUPAL_AUTHENTICATED_RID] = 'authenticated user';
  
  // Get the optional lists from MailChimp.
  $lists = mailchimp_lists_get_available_lists($a,
    MAILCHIMP_LISTTYPE_OPTIONAL, array('show_register_form' => 1));

  if (!empty($lists)) {
    $i = 0;
    foreach ($lists as $list) {
      mailchimp_lists_auth_newsletter_form($element, $list, $account);
      $wrapper_key = 'mailchimp_' . $list->id;
      
      if (!$field['show_lists']) {
        // If this field should be hidden, don't display it
        if ($field['key'] == 'mc_list_hidden') {
          $element[$wrapper_key]['subscribe']['#type'] = 'value';
          unset($element[$wrapper_key]['interest_groups']);
        }
      }
      
      // If this user is already subscribed, remove the option
      if ($element[$wrapper_key]['subscribe']['#default_value']) {
        if (!$field['show_selected']) {
          $element[$wrapper_key]['subscribe']['#access'] = FALSE;
        }
      }
      
      // Select the first list, if requested
      if ($i == 0 && $field['select_first']) {
        $element[$wrapper_key]['subscribe']['#default_value'] = 1;
      }
      $i++;
    }
  }
  return $element;
}

function pseudo_login_mc_field_form_submit($field, &$element, &$form_state, &$account, $values) {
  foreach ($values as $wrapper_key => $vals) {
    $list = $vals['list'];
    $interests = array();
    
    if (isset($vals['interest_groups'])) {
      $interests = $vals['interest_groups'];
    }
    if ($vals['subscribe']) {
      pseudo_login_mc_signup($account, $list, $interests);
    }
  }
}