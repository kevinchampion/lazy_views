<?php
/**
 * @file Views plugin
 */

/**
 * Implements hook_views_api().
 */
function lazy_views_views_api() {
  return array(
    'api' => '3.0',
    'path' => drupal_get_path('module', 'lazy_views'),
  );
}

/**
 * Implements hook_views_plugins().
 */
function lazy_views_views_plugins() {
  return array(
    'cache' => array(
      'lazy_views' => array(
        'title' => t('Lazy views caching'),
        'handler' => 'views_plugin_cache_lazy_views',
        'uses options' => TRUE,
        'path' => drupal_get_path('module', 'lazy_views') . '/plugins/views', // not necessary for most modules
      ),
    ),
  );
}
