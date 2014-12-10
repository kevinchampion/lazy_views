<?php

/**
 * @file
 * Contains the 'lazy_views' cache plugin.
 */

/**
 * Simple caching of query results for Views displays.
 *
 * @ingroup views_cache_plugins
 */
class views_plugin_cache_lazy_views extends views_plugin_cache_time {

  /**
   * Extends views_plugin_cache::options_form().
   *
   * Adds lazy_views specific settings to cache time settings.
   */
  function option_definition() {

    $options = parent::option_definition();

    $options['show_spinner'] = array('default' => 0);
    $options['show_text'] = array('default' => 0);
    $options['placeholder_text'] = array('default' => '');

    return $options;

  }

  /**
   * Extends views_plugin_cache::options_form().
   *
   * Adds lazy_views specific settings to cache time settings.
   */
  function options_form(&$form, &$form_state) {

    parent::options_form($form, $form_state);

    $form['show_spinner'] = array(
      '#type' => 'checkbox',
      '#title' => t('Spinner'),
      '#description' => t('Select if you\'d like a graphical spinner icon to display in the placeholder element until the view content is lazy-loaded and placed.'),
      '#default_value' => $this->options['show_spinner'],
    );
    $form['show_text'] = array(
      '#type' => 'checkbox',
      '#title' => t('Placeholder text'),
      '#description' => t('Select if you\'d like placeholder text to display in the placeholder element until the view content is lazy-loaded and placed.'),
      '#default_value' => $this->options['show_text'],
    );
    $form['placeholder_text'] = array(
      '#type' => 'textarea',
      '#title' => t('Placeholder text'),
      '#description' => t('Enter text here to display in the placeholder element until the view content is lazy-loaded and placed.'),
      '#default_value' => $this->options['placeholder_text'],
      '#process' => array('form_process_select','ctools_dependent_process'),
      '#dependency' => array(
        'edit-cache-options-show-text' => array(1),
      ),
    );

  }

  /**
   * Overrides views_plugin_cache::get_output_key().
   *
   * @return string
   *   Completed cache id.
   */
  function get_output_key() {

    if (!isset($this->_output_key)) {

      $key_data = array(
        'theme' => $GLOBALS['theme'],
      );
      $this->_output_key = 'lazy_views:' . $this->view->name . ':' . $this->display->id . ':output:' . $this->get_cache_key($key_data);

    }

    return $this->_output_key;

  }

  /**
   * Overrides views_plugin_cache::get_cache_key().
   *
   * Returns cache key.
   *
   * @param array $key_data
   *   Additional data for cache segmentation and/or overrides for default
   *   segmentation.
   *
   * @return string
   *   Md5 hash of serialized data specific to this view request.
   */
  function get_cache_key($key_data = array()) {
    global $user;

    $key_data += array(
      'roles' => array_keys($user->roles),
      'super-user' => $user->uid == 1, // special caching for super user.
      'language' => $GLOBALS['language']->language,
      'base_url' => $GLOBALS['base_url'],
      'arguments' => $this->view->args,
    );

    // Need to alter what would normally be build_info data as part of cache key
    // because need to be able to retrieve it when the view is not yet processed
    // on lazy-load. As a result, a simplified verison is used by just making
    // use of the arguments, which can conceivably be retrieved on a
    // pre-processed view.

    /*if (empty($key_data['build_info'])) {
      $build_info = $this->view->build_info;
      foreach (array('query','count_query') as $index) {
        // If the default query back-end is used generate SQL query strings from
        // the query objects.
        if ($build_info[$index] instanceof SelectQueryInterface) {
          $query = clone $build_info[$index];
          $query->preExecute();
          $key_data['build_info'][$index] = array(
            'sql' => (string) $query,
            'arguments' => $query->getArguments(),
          );
        }
      }
    }*/
    $key = md5(serialize($key_data));
    return $key;
  }

}
