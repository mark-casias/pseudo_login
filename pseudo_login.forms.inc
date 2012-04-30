<?php
/**
 * @file 
 * 
 * Copyright 2012 New Signature
 * http://www.newsignature.com
 *
 * @author Andrew Marcus
 * @since Apr 6, 2012
 */

/**
 * Creates a full signup form, using the pseudo_login module.
 */
function pseudo_login_signup_page_form($form, &$form_state) {
  $title = variable_get('pseudo_login_signup_page_title', '');
  if (!empty($title)) {
    drupal_set_title($title);
  }
  
  $intro = variable_get('pseudo_login_signup_page_intro', array()) + array(
    'value' => '',
    'format' => 'full_html',
  );
  
  if (!empty($intro['value'])) {
    $form['intro'] = array(
    	'#markup' => check_markup($intro['value'], $intro['format']),
      '#prefix' => '<div class="intro">',
      '#suffix' => '</div>',
    );
  }
  pseudo_login_attach_form($form, $form_state, 'pseudo_login_signup_page_form');
  
  $form['actions'] = array(
    '#type' => 'actions',
  
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Sign Up!'),
    ),
  );
  
  $privacy = variable_get('pseudo_login_signup_page_privacy', '');
  if (!empty($privacy)) {
    $form['actions']['privacy'] = array(
      '#type' => 'link',
      '#href' => $privacy,
      '#title' => t('Privacy policy'),
    );
  }
  
  return $form;
}

function pseudo_login_signup_page_form_validate(&$form, &$form_state) {
  pseudo_login_attach_form_validate($form, $form_state);
}

function pseudo_login_signup_page_form_submit(&$form, &$form_state) {
  pseudo_login_attach_form_submit($form, $form_state);
  
  $account = $form_state['user'];
  $form_state['redirect'] = variable_get('pseudo_login_signup_page_redirect', '<front>');
}

function pseudo_login_signup_page_edit_form($form, &$form_state) {
  $title = variable_get('pseudo_login_signup_page_title', t('Receive Email Updates'));
  $form['pseudo_login_signup_page_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Page title'),
    '#default_value' => $title,
  );
  
  $current = variable_get('pseudo_login_signup_page_intro', array()) + array(
    'value' => '',
    'format' => 'full_html',
  );
  $form['pseudo_login_signup_page_intro'] = array(
    '#type' => 'text_format',
    '#title' => t('Intro text'),
    '#rows' => 6,
    '#description' => t('The text to appear at the top of the form.'),
    '#default_value' => $current['value'],
    '#format' => $current['format'],
  );
  
  $form['pseudo_login_signup_page_redirect'] = array(
    '#type' => 'textfield',
    '#title' => t('Redirect path'),
    '#description' => t('The Drupal path to redirect users to after they have signed up.  Use <front> for the home page.'),
    '#default_value' => variable_get('pseudo_login_signup_page_redirect', '<front>'),
  );
  
  $form['pseudo_login_signup_page_privacy'] = array(
    '#type' => 'textfield',
    '#title' => t('Privacy Policy link'),
    '#description' => t('The Drupal path to the privacy policy page.  Leave blank to omit this link.'),
    '#default_value' => variable_get('pseudo_login_signup_page_privacy', ''),
  );
  
  $form['#submit'][] = 'pseudo_login_signup_page_edit_form_submit';
  
  return system_settings_form($form);
}

function pseudo_login_signup_page_edit_form_submit($form, &$form_state) {
  $form_state['redirect'] = 'signup';
}