<?php
/**
 * @file
 * Defines hooks for module.
 */

/**
 * Defines hook_pseudo_login_form_info().
 *
 * Defines one or more forms that pseudo-login functionality can be attached to.
 * The key of each info block should be a valid form_ID as used
 * by drupal_get_form().
 *
 * You must also call pseudo_login_attach_form(),
 * pseudo_login_attach_form_validate() and pseudo_login_attach_form_submit()
 * within your form functions in order to attach the appropriate functionality.
 *
 * @return array
 *   Form info array.
 */
function hook_pseudo_login_form_info() {
  // From the donations module.
  return array(
    // Keyed by the form_ID
    'donations_view_form' => array(
      // The title of this form for admins.
      'title' => t('Donation Page'),
      // The title to display on the user form for this group of fields.
      'fieldset_title' => t('Billing Information'),

      // A definition of the fields to display in our form, along with an
      // initial attempt at mapping them to user fields. The key name will be
      // used to store the value internally, and the value should correspond to
      // a field defined by hook_pseudo_login_field_info().
      // This mapping can be edited by admins.
      'fields' => array(
        'email' => 'mail',
        'title' => 'field_user_title',
        'first_name' => 'field_first_name',
        'last_name' => 'field_last_name',
        'suffix' => 'field_user_suffix',
        'address' => 'field_user_address',
      ),
      // Which of the fields are required?
      'required' => array(
        'email', 'first_name', 'last_name', 'address',
      ),
      // Specify a source code to connect to users created through this form.
      // This can be edited by admins.
      'source' => 'webform-donation',
    ),
  );
}

/**
 * Defines hook_pseudo_login_field_info().
 *
 * Define one or more fields that pseudo-login forms can present to users.
 * By default, all regular fields are passed through as-is, but additional
 * functionality can be added here.
 *
 * For instance, a special field definition is used for email fields, in order
 * to properly handle logging in users.
 */
function pseudo_login_pseudo_login_fields_info() {
  $info = array();

  // The keys presented here are used in the field mappings in
  // hook_pseudo_login_form_info().
  $info['mail'] = array(

    // The type of entity that provides this field (displayed to admins).
    'type' => 'User',
    // The title of the field (displayed to admins).
    'title' => 'Email',
    // Optional: base name to use for callbacks, see module_info_get_callback()
    'base' => 'pseudo_login_field_mail',

    // The location of the callback to render the form element for this field.
    // If not provided, it will be assumed based on the 'base' or module name.
    'callbacks' => array(
      'form' => 'pseudo_login_field_mail_form',
    ),
  );
  $info['mail_login'] = array(
    'type' => 'User',
    'title' => 'Email with login form',
    'base' => 'pseudo_login_field_mail',
  );

  // Create a pseudo-login field for each field attached to the User entity.
  $fields = field_info_instances('user', 'user');
  foreach ($fields as $field_name => $field) {
    $info[$field_name] = array(
      'type' => 'User',
      'title' => $field['label'],
      'base' => 'pseudo_login_field_user_field',
      'entity' => 'user',
      'bundle' => 'user',
      'instance' => $field,
    );
  }

  return $info;
}