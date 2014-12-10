
/**
 * @file
 * Responsible for tracking lazy views and replacing them through AJAX
 */

(function ($) {
  "use strict";

  var lazyViewsAJAX = {},
      lazyViews = [];

  // Register to events
  // --------------------------------------------------------------------------

  Drupal.behaviors.lazyViewsLoad = {
    attach: function () {

      var $lazy_views = $('.lazy-views-placeholder').not('.processed'),
          ids = [];

      if ($lazy_views.length) {
        $.each($lazy_views, function () {

          $('.lazy-views-spinner').spin();

          var id = $(this).data('lazy-views-cache-id');

          ids.push(id);

          $(this).addClass('processed');

        });

        lazyViewsAJAX.request(ids);

      }
    }
  };

  // lazyViewsAJAX
  // --------------------------------------------------------------------------

  /**
   * Makes an AJAX request to load the lazy views.
   *
   * @param ids
   *  An array of lazy-views ids.
   */
  lazyViewsAJAX.request = function (ids) {

    if (!ids.length) {
      return;
    }

    var url = '/lazy-views/ajax',
        data = $.extend({'lazy_views_ids[]': ids}, {'lazy_views_get' : this.getURLParams()});

    data['ajax_page_state[theme]'] = Drupal.settings.ajaxPageState.theme;
    data['ajax_page_state[theme_token]'] = Drupal.settings.ajaxPageState.theme_token;
    data['lazy_views_current_path'] = Drupal.settings.lazy_views.current_path;
    data['lazy_views_arguments'] = Drupal.settings.lazy_views.view_arguments;

    for (var key in Drupal.settings.ajaxPageState.css) {
      data['ajax_page_state[css][' + key + ']'] = 1;
    }

    for (var key in Drupal.settings.ajaxPageState.js) {
      data['ajax_page_state[js][' + key + ']'] = 1;
    }

    $.post(url, data, function (response, status) {
      lazyViewsAJAX.success(response, status);
    }, 'json');
  };

  /**
   * Processes a successful lazy-views AJAX response.
   */
  lazyViewsAJAX.success = function (response, status) {

    Drupal.freezeHeight();

    for (var i in response) {
      if (response[i]['command'] && this.commands[response[i]['command']]) {
        this.commands[response[i]['command']](response[i], status);
      }
    }

    Drupal.unfreezeHeight();

  };

  /**
   * An object that hosts execution commands supported by lazy-views.
   */
  lazyViewsAJAX.commands = {

    insert: function (response, status) {
      var $wrappers = $(response.selector),
          method = response.method,
          settings = response.settings || Drupal.settings;

      $wrappers.each(function (index, el) {
        var $wrapper = $(el),
            $wrapped_contents = $('<div></div>').html(response.data).hide(),
            contents = $wrapped_contents.contents();

        $wrapper.fadeOut(500, function () {
          $wrapper[method]($wrapped_contents);
          $wrapped_contents.fadeIn(600, function () {
            $wrapped_contents.replaceWith(contents);
            Drupal.attachBehaviors(contents, settings);
          });
        });
      });
    },

    settings: function (response, status) {
      if (response.merge) {
        $.extend(true, Drupal.settings, response.settings);
      }
    }

  };

  /**
   * Extracts GET params from the URL.
   */
  lazyViewsAJAX.getURLParams = function () {

    var location = document.location.search,
        params = {},
        tokens,
        regexp = /[?&]?([^=]+)=([^&]*)/g;

    while ((tokens = regexp.exec(location)) !== null) {
      params[decodeURIComponent(tokens[1])] = decodeURIComponent(tokens[2]);
    }

    return params;

  };

}(jQuery));
