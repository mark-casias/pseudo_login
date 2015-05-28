<?php
/**
 * @file
 */

/**
 * Implements pseudo_login_field_info().
 */
function pseudo_login_pseudo_login_fields_info() {
  $info = array();
  
  $info['mail'] = array(
    'type' => t('User'),
    'title' => t('Email'),
    'base' => 'pseudo_login_field_mail',
  );
  $info['mail_login'] = array(
    'type' => t('User'),
    'title' => 'Email with login form',
    'base' => 'pseudo_login_field_mail',
  );
  
  // Add user fields
  $fields = field_info_instances('user', 'user');
  foreach ($fields as $field_name => $field) {
    $info[$field_name] = array(
      'type' => t('User'),
      'title' => $field['label'],
      'base' => 'pseudo_login_field_user_field',
      'entity' => 'user',
      'bundle' => 'user',
      'instance' => $field,
    );
  }
  return $info;
}

function pseudo_login_field_mail_form($field, &$form_state, $account, $required = TRUE) {
  $element = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array( 'pseudo-login-email-wrapper' ),
    ),
    '#attached' => array( 'css' => array( drupal_get_path('module', 'pseudo_login') . '/css/email.css' )),
  );
  
  if ($account->uid > 0) {
    $element['name'] = array(
      '#type' => 'item',
      '#title' => t('E-mail'),
      '#markup' => '<div class="email-readonly">' . check_plain($account->mail) . '</div>',
      '#value' => $account->mail,
      '#input' => TRUE,
    );
    
    // If this is a pseudo-user, provide options to log in or log out.
    if (user_is_pseudo($account)) {
      $element['name']['#description'] = t('If this is not you, <a href="!url">click here</a>.', array(
        '!url' => url('user/logout/pseudo', array( 'query' => array( 'destination' => $_GET['q']) ) ),
      ));
    }
  }
  else {
    $element['name'] = array(
      '#type' => 'textfield',
      '#title' => t('E-mail'),
      '#element_validate' => array( 'pseudo_login_field_mail_validate' ),
      '#default_value' => '',
      '#required' => TRUE, // The email field is always required
    );
  }
  
  if (!user_is_logged_in() && $field['key'] == 'mail_login') {
    $element['pass'] = array(
      '#type' => 'password',
      '#title' => t('Enter your password (optional)'),
      '#description' => t('To load additional values from your account, login here.')
    );
    $element['login'] = array(
      '#type' => 'submit',
      '#value' => t('Login'),
      '#name' => 'login',
      '#submit' => array( 
        'user_login_submit',
        'pseudo_login_field_mail_login_submit',
      ),
    );
    $element['#element_validate'] = array(
      'pseudo_login_field_mail_login_validate',
    );
    $element['#process'] = array( 
      'pseudo_login_field_mail_login_process', 
    );
  }
  
  form_load_include($form_state, 'inc', 'pseudo_login', 'pseudo_login.pseudo');
  return $element;
}

function pseudo_login_field_mail_validate($element, &$form_state) {
  if (!valid_email_address($element['#value'])) {
    form_error($element, t('Please enter a valid email address.'));
  }
}

function pseudo_login_field_mail_form_validate($field, &$element, &$form_state, &$account, $values) {
  $mail = $values['name'];
  
  // If the user is not logged in, attempt to match them by the provided email.
  if (empty($account->uid)) {
    
    // If a matching user was found by email address, use them
    if ($uid = db_query("SELECT uid FROM {users} WHERE LOWER(mail) = LOWER(:mail)", array(
        ':mail' => $mail,
      ))->fetchField()) {
      $account = user_load($uid);
    }
    else {
      $account = (object)array(
        'is_new' => TRUE,
        'uid' => 0,
        'name' => $mail,
        'mail' => $mail,
        'init' => $mail,
        'pass' => user_password(8), // Generate a hard-to-guess password
        'status' => 1,
        'access' => REQUEST_TIME,
      );
    }
  }
  
  return $mail;
}

function pseudo_login_field_mail_login_validate(&$element, &$form_state) {
  if ($form_state['triggering_element']['#id'] == $element['login']['#id']) {
    $mail = $element['name']['#value'];
    $pass = $element['pass']['#value'];
      
    if (!empty($mail) && !empty($pass)) {
      if ($name = db_query("SELECT name FROM {users} WHERE LOWER(mail) = LOWER(:name)", array(
        ':name' => $mail,
      ))->fetchField()) {
        $form_state['values']['name'] = $name;
        $form_state['values']['pass'] = $pass;
        
        user_login_authenticate_validate($element, $form_state);
        user_login_final_validate($element, $form_state);
      }
      else {
        form_error($element['pass'], t('You don\'t have an account yet. But once you donate, you can register on this site.'));
      }
    }
  }
}

function pseudo_login_field_mail_login_process(&$element, &$form_state) {
  $element['login']['#limit_validation_errors'] = array(
    $element['#parents'],
    array('name'),
    array('pass'),
  );
  return $element;
}

function pseudo_login_field_mail_login_submit($form, &$form_state) {
  drupal_set_message(t('Welcome, you are now logged in.'));
  unset($form_state['redirect']);
}


function pseudo_login_field_user_field_form($field, &$form_state, $account, $required = FALSE) {
  $elements = &drupal_static(__FUNCTION__);
  
  // Render all of the form elements for this entity, and choose the requested one
  if (!isset($elements)) {
    $elements = array();
    field_attach_form($field['entity'], $account, $elements, $form_state);
  }
  
  // Attach the requested form element, if we can
  $field_name = $field['key'];
  if (isset($elements[$field_name])) {
    $element = $elements[$field_name];
    $lang = $element['#language'];
    
    unset($element['#weight']);
    $element['#attributes']['class'][] = drupal_html_class('pseudo-login-' . $field_name);
    
    if ($required) {
      $element['#required'] = TRUE;
      
      foreach (element_children($element[$lang][0]) as $key) {
        $element[$lang][0][$key]['#required'] = TRUE;
      }
    }
    
    // If this is an address field, hide Street 1 and 2 for privacy reasons unless the user is logged in.
    if ($field['instance']['widget']['module'] == 'addressfield') {
      if (user_is_pseudo($account) && !user_is_logged_in()) {
        $element[$lang][0]['#address']['thoroughfare'] = '';
        $element[$lang][0]['#address']['premise'] = '';
      }
      // The Street 2 field is never required
      $element[$lang][0]['street_block']['premise']['#required'] = FALSE;
    }
    return $element;
  }
  return FALSE;
}


function pseudo_login_field_user_field_form_validate($field, &$element, &$form_state, &$user, $values) {
  // Update the user account
  $user->$field['key'] = $values;
  
  $lang = $element['#language'];
  $values = $values[$lang];
  
  // If there is only one value for each sub-field, unnest it. 
  foreach ($values as $delta => $v) {
    if (is_array($v) && count($v) == 1) {
      $values[$delta] = reset($v);
    }
  }
  
  // If there is only one value for this field, return it directly.
  if (count($values) == 1) {
    return reset($values);
  }
  
  // Otherwise, return an array of values for this field.
  return $values;
}
