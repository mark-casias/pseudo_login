<?php
/**
 * @file
 */

/**
 * Implements hook_entity_property_info().
 */
function pseudo_login_entity_property_info() {
  return array();
}

/**
 * Implements hook_entity_property_info_alter().
 * 
 * @param array $info The info array.
 */
function pseudo_login_entity_property_info_alter(&$info) {
  if (isset($info['user']['properties'])) {
    $info['user']['properties']['pseudo_login_token'] = array(
      'label' => t('Pseudo-Login Token'),
      'description' => t('The token that will let this user pseudo-login.'),
      'type' => 'text',
      'getter callback' => 'entity_property_verbatim_get',
      'setter callback' => 'entity_property_verbatim_set',
    );
    $info['user']['properties']['source'] = array(
      'label' => t('Source'),
      'description' => t('The source of this user in the system'),
      'type' => 'text',
      'getter callback' => 'pseudo_login_entity_property_get',
    );
  }
}