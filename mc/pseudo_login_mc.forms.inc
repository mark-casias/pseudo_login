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
 * Creates a small signup form for the site footer.
 */
function pseudo_login_mc_signup_form($form, &$form_state) {
  $form['signup'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'id' => 'signup-form-wrapper',
    ),
  );
  
  $form['signup'] += array(
    'email' => array(
      '#type' => 'textfield',
      '#id' => 'email',
      '#attributes' => array(
        'placeholder' => t('Enter your email'),
      ),
      '#element_validate' => array(
        'pseudo_login_mc_signup_form_validate_email',
      ),
    ),
    'submit' => array(
      '#type' => 'submit',
      '#id' => 'mc-join',
      '#attributes' => array(
        'class' => array( 'btn' ),
      ),
      '#value' => t('▸'),
      '#submit' => array('pseudo_login_mc_signup_form_submit'),
    ),
  );
  
  // If we are not redirecting anywhere, submit the form via AJAX.
  $redirect = variable_get('pseudo_login_mc_signup_redirect', '');
  if (empty($redirect)) {
    $form['signup']['submit'] += array(
      '#ajax' => array(
        'callback' => 'pseudo_login_mc_signup_form_ajax',
        'wrapper' => 'signup-form-wrapper',
        'progress' => array( 'type' => 'throbber', 'message' => '' ),
      ),
    );
  }
  
  form_load_include($form_state, 'inc', 'pseudo_login_mc', 'pseudo_login_mc.forms');
  return $form;
}

function pseudo_login_mc_signup_form_ajax($form, $form_state) {
  if (!empty($form_state['redirect'])) {
    $cmds = array();
    $cmds[] = ajax_command_replace('signup-form-wrapper', '');
    $cmds[] = ajax_command_invoke('form#pseudo-login-mc-signup-form', 'submit');
    return $cmds;
  }
  return $form['signup'];
}

function pseudo_login_mc_signup_form_validate_email(&$element, &$form_state) {
  if (empty($element['#value'])) {
    form_error($element, t('You did not specify an email address to sign up with.'));
  }
  if (!valid_email_address($element['#value'])) {
    form_error($element, t('%value is not a valid email address.', array(
      '%value' => $element['#value'],
    )));
  }
}

function pseudo_login_mc_signup_form_submit($form, &$form_state) {
  $email = $form_state['values']['email'];
  
  // Try to find the user with this email address.
  $account = user_load_by_mail($email);
  if (!$account) {
    $account = new stdClass();
    $edit = array(
      'mail' => $email,
      'name' => $email,
      'init' => $email,
      'data' => array( 'source' => 'webform-signup' ),
      'pass' => user_password(8),
    );
    user_save($account, $edit);
  }
  
  // Subscribe the user to the default list
  $is_subscribed = pseudo_login_mc_signup($account);
  
  // If the user is not logged in, do a pseudo login for them.
  pseudo_login($account);
    
  $redirect = variable_get('pseudo_login_mc_signup_redirect', '');
  if (!empty($redirect)) {
    if ($redirect == 'signup') {
      drupal_set_message(t('Please help us out by providing additional information below.'));
    }
    $form_state['redirect'] = $redirect;
  }
  else {
    // Otherwise, the form was submitted via AJAX so rebuild it.
    $form_state['rebuild'] = TRUE;
  }
}
