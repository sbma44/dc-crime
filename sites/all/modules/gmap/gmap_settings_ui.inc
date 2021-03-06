<?php
// $Id: gmap_settings_ui.inc,v 1.1.2.38 2008/10/16 18:45:34 bdragon Exp $

/**
 * @file
 * GMap settings form.
 */

function _gmap_admin_settings() {
  // Reset the icondata cache.
  gmap_get_icondata(TRUE);

  //note the same google api key variable name as in the googlemap module is used
  //note the name of the variable for center of the map is latlong although the format is actually longitude, latitude

  $form['initialization'] = array(
    '#type' => 'fieldset',
    '#title' => t('Google Map Initialize'),
  );
  if (!module_exists('keys_api')) {
    $form['initialization']['googlemap_api_key'] = array(
      '#type' => 'textfield',
      '#title' => t('Google Maps API Key'),
      '#default_value' => variable_get('googlemap_api_key', ''),
      '#size' => 50,
      '#maxlength' => 255,
      '#description' => t('Your personal Googlemaps API key.  You must get this for each separate website at <a href="http://www.google.com/apis/maps/">Google Map API website</a>.'),
    );
  }
  else {
    $form['initialization']['googlemap_api_key'] = array(
      '#type' => 'item',
      '#title' => t('Google Maps API Key'),
      '#description' => t('Your personal Googlemaps API key.  You must get this for each separate website at <a href="http://www.google.com/apis/maps/">Google Map API website</a>.'),
      '#value' => t("Managed by <a href='@url'>keys api</a>.", array('@url' => url('admin/settings/keys'))),
    );
  }
  $form['initialization']['gmap_load_zoom_plugin'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable mousewheel zooming'),
    '#default_value' => variable_get('gmap_load_zoom_plugin', FALSE),
    '#description' => t('Enables the mousewheel.js jQuery plugin, which adds the ability to use the mouse wheel to zoom the map.'),
  );
  if (!file_exists(drupal_get_path('module', 'gmap') .'/thirdparty/mousewheel.js')) {
    variable_set('gmap_load_zoom_plugin', FALSE);
    $form['initialization']['gmap_load_zoom_plugin']['#default_value'] = FALSE;
    $form['initialization']['gmap_load_zoom_plugin']['#value'] = FALSE;
    $form['initialization']['gmap_load_zoom_plugin']['#disabled'] = TRUE;
    $form['initialization']['gmap_load_zoom_plugin']['#description'] += '<br />'. t('<em>The mousewheel.js file was not found. Please download and place in the <b>thirdparty/</b> folder.</em>');
  }

  if (variable_get('file_downloads', FILE_DOWNLOADS_PUBLIC) != FILE_DOWNLOADS_PUBLIC) {
    $form['initialization']['gmap_private_markerfile'] = array(
      '#type' => 'textfield',
      '#title' => t('Path to gmap_markers.js'),
      '#description' => t('You are using the <em>Private</em> download method. For markers to work properly. you must press the <em>Regenerate</em> button, manually copy js/gmap_markers.js from the files directory to a location accessible by the webserver, and enter the file path relative to the Drupal root in this field.'),
      '#default_value' => variable_get('gmap_private_markerfile', ''),
    );
  }

  $form['initialization']['rebuild'] = array(
    '#type' => 'fieldset',
    '#title' => t('Regenerate marker cache'),
    '#description' => t('If you are having problems with markers, or have modified the .ini files in the markers folder, click here to rebuild the marker cache file.'),
  );

  $form['initialization']['rebuild']['rebuild_marker_js'] = array(
    '#type' => 'submit',
    '#value' => t('Regenerate'),
  );

  $defaults = gmap_defaults();

  $form['gmap_default'] = array(
    '#type' => 'fieldset',
    '#title' => t('Default map settings'),
    // This will store all the defaults in one variable.
    '#tree' => TRUE,
  );
  $form['gmap_default']['map'] = array(
    '#type' => 'gmap',
    '#map' => 'settings_default_map',
    '#settings' => array(
      'behavior' => array(
        'nodrag' => FALSE,
        'nokeyboard' => FALSE,
      ),
    ),
  );

  // Allow previewable behaviors to affect the preview map.
  $m = array();
  $behaviors = module_invoke_all('gmap', 'behaviors', $m);
  foreach ($behaviors as $k => $v) {
    if (isset($v['previewable']) && $v['previewable']) {
      $form['gmap_default']['map']['#settings']['behavior'][$k] = $defaults['behavior'][$k];
    }
  }

  $form['gmap_default']['width'] = array(
    '#type' => 'gmap_dimension',
    '#title' => t('Default width'),
    '#default_value' => $defaults['width'],
    '#size' => 25,
    '#maxlength' => 25,
    '#description' => t('The default width of a Google map, as a CSS length or percentage. Examples: <em>50px</em>, <em>5em</em>, <em>2.5in</em>, <em>95%</em>'),
  );
  gmap_widget_setup($form['gmap_default']['width'], 'width', 'settings_default_map');
  $form['gmap_default']['height'] = array(
    '#type' => 'gmap_dimension',
    '#title' => t('Default height'),
    '#default_value' => $defaults['height'],
    '#size' => 25,
    '#maxlength' => 25,
    '#description' => t('The default height of a Google map, as a CSS length or percentage. Examples: <em>50px</em>, <em>5em</em>, <em>2.5in</em>, <em>95%</em>'),
  );
  gmap_widget_setup($form['gmap_default']['height'], 'height', 'settings_default_map');
  $form['gmap_default']['latlong'] = array(
    '#type' => 'gmap_latlon',
    '#map' => 'settings_default_map',
    '#title' => t('Default center'),
    '#default_value' => $defaults['latlong'],
    '#size' => 50,
    '#maxlength' => 255,
    '#description' => t('The default center coordinates of Google map, expressed as a decimal latitude and longitude, separated by a comma.'),
  );
  $form['gmap_default']['zoom'] = array(
    '#type' => 'select',
    '#title' => t('Default zoom'),
    '#default_value' => $defaults['zoom'],
    '#options' => drupal_map_assoc(range(0, 17)),
    '#description' => t('The default zoom level of a Google map.'),
  );
  gmap_widget_setup($form['gmap_default']['zoom'], 'zoom', 'settings_default_map');
  $form['gmap_default']['maxzoom'] = array(
    '#type' => 'select',
    '#title' => t('Maximum initial zoom'),
    '#default_value' => $defaults['maxzoom'],
    '#options' => drupal_map_assoc(range(0, 17)),
    '#description' => t('The maximum initial zoom (affects things such as the zoom of the node location block.)'),
  );
  // This one isn't a gmap widget.

  $form['gmap_default']['styles']['line_default'] = array(
    '#type' => 'gmap_style',
    '#gmap_style_type' => 'line',
    '#title' => t('Line default style'),
    '#description' => t('Lines without a specific style defined will fall back to this style'),
    '#default_value' => $defaults['styles']['line_default'],
  );
  $form['gmap_default']['styles']['poly_default'] = array(
    '#type' => 'gmap_style',
    '#gmap_style_type' => 'poly',
    '#title' => t('Polygon default style'),
    '#description' => t('Polygons without a specific style defined will fall back to this style'),
    '#default_value' => $defaults['styles']['poly_default'],
  );

  $form['gmap_default']['controltype'] = array(
    '#type' => 'select',
    '#title' => t('Default control type'),
    '#options' => array('None' => t('None'), 'Micro' => t('Micro'), 'Small' => t('Small'), 'Large' => t('Large')),
    '#default_value' => $defaults['controltype'],
  );
  gmap_widget_setup($form['gmap_default']['controltype'], 'controltype', 'settings_default_map');

  $form['gmap_default']['mtc'] = array(
    '#type' => 'select',
    '#title' => t('Map Type Control'),
    '#options' => array(
      'none' => t('None'),
      'standard' => t('Standard (GMapTypeControl)'),
      'hier' => t('Hierarchical (GHierarchicalMapTypeControl)'),
      'menu' => t('Dropdown (GMenuMapTypeControl)'),
    ),
    '#default_value' => $defaults['mtc'],
  );

  $form['gmap_default']['baselayers'] = array(
    '#type' => 'fieldset',
    '#title' => t('Enabled map types ("base layers")'),
  );

  $baselayers = array();

  foreach (module_implements('gmap') as $module) {
    call_user_func_array($module .'_gmap', array('baselayers', &$baselayers));
  }

  $options = array();
  foreach ($baselayers as $name => $layers) {
    $options[$name] = array();
    foreach ($layers as $k => $v) {
      // @@@TODO: Only show the enabled ones?
      $options[$name][$k] = $v['title'];
    }
  }

  $form['gmap_default']['baselayers']['maptype'] = array(
    '#type' => 'select',
    '#tree' => FALSE,
    '#parents' => array('gmap_default', 'maptype'),
    '#title' => t('Default map type'),
    '#default_value' => $defaults['maptype'],
    '#options' => $options,
  );
  gmap_widget_setup($form['gmap_default']['baselayers']['maptype'], 'maptype', 'settings_default_map');

  foreach ($baselayers as $name => $layers) {
    $form['gmap_default']['baselayers'][$name] = array(
      '#type' => 'fieldset',
      '#title' => $name,
    );
    foreach ($layers as $key => $layer) {
      $form['gmap_default']['baselayers'][$name][$key] = array(
        '#type' => 'checkbox',
        '#title' => $layer['title'],
        '#parents' => array('gmap_default', 'baselayers', $key),

        '#description' => $layer['help'],
        '#default_value' => isset($defaults['baselayers'][$key]) ? $defaults['baselayers'][$key] : $layer['default'],
      );
    }
  }

  $form['gmap_default']['behavior'] = array(
    '#type' => 'fieldset',
    '#title' => t('Map Behavior flags'),
    '#tree' => TRUE,
    '#description' => t('Behavior flags modify how a map behaves. Grayed out flags are not settable here, but may be set on a map by map basis via code or a macro. Changes to behaviors will not affect the preview map shown above until changes are saved.'),
  );
  $m = array();
  $behaviors = module_invoke_all('gmap', 'behaviors', $m);
  foreach ($behaviors as $k => $v) {
    $form['gmap_default']['behavior'][$k] = array(
      '#type' => 'checkbox',
      '#title' => t('@name : @title', array('@name' => $k, '@title' => $v['title'])),
      '#default_value' => isset($defaults['behavior'][$k]) ? $defaults['behavior'][$k] : $v['default'],
      '#description' => isset($v['help']) ? $v['help'] : '',
    );
    if (isset($v['internal']) && $v['internal']) {
      $form['gmap_default']['behavior'][$k]['#disabled'] = TRUE;
      // Compensate for behaviors becoming internal after acquiring the wrong value.
      $form['gmap_default']['behavior'][$k]['#value'] = $v['default'];
    }
  }

  $form['gmap_default']['markermode'] = array(
    '#type' => 'radios',
    '#title' => t('Marker action'),
    '#description' => t('Perform this action when a marker is clicked.'),
    '#options' => array(t('Do nothing'), t('Open info window'), t('Open link')),
    '#default_value' => isset($defaults['markermode']) ? $defaults['markermode'] : 0,
  );

  $form['gmap_default']['line_colors'] = array(
    '#tree' => TRUE,
  );
  $form['gmap_default']['line_colors'][0] = array(
    '#type' => 'textfield',
    '#title' => t('Default Line 1 Color'),
    '#default_value' => $defaults['line_colors'][0],
    '#size' => 12,
    '#maxlength' => 7,
  );
  $form['gmap_default']['line_colors'][1] = array(
    '#type' => 'textfield',
    '#title' => t('Default Line 2 Color'),
    '#default_value' => $defaults['line_colors'][1],
    '#size' => 12,
    '#maxlength' => 7,
  );
  $form['gmap_default']['line_colors'][2] = array(
    '#type' => 'textfield',
    '#title' => t('Default Line 3 Color'),
    '#default_value' => $defaults['line_colors'][2],
    '#size' => 12,
    '#maxlength' => 7,
  );

  $opts = variable_get('gmap_markermanager', array());
  if (!isset($opts['gmap']) || !is_array($opts['gmap'])) {
    $opts['gmap'] = array();
  }
  if (!isset($opts['gmarkermanager']) || !is_array($opts['gmarkermanager'])) {
    $opts['gmarkermanager'] = array();
  }
  if (!isset($opts['clusterer']) || !is_array($opts['clusterer'])) {
    $opts['clusterer'] = array();
  }

  $opts['gmap'] = array_merge(array(
    // None.
  ), $opts['gmap']);

  $opts['gmarkermanager'] = array_merge(array(
    'borderPadding' => 0,
    'maxZoom' => 4,
    'trackMarkers' => FALSE,
    'markerMinZoom' => 4,
    'markerMaxZoom' => 0,
  ), $opts['gmarkermanager']);

  $opts['clusterer'] = array_merge(array(
    'clusterer_file' => 'Clusterer2.js',
    'marker' => 'cluster',
    'max_nocluster' => 150,
    'cluster_min' => 5,
    'max_lines' => 10,
  ), $opts['clusterer']);

  $form['gmap_markermanager'] = array(
    '#type' => 'fieldset',
    '#title' => t('Marker manager'),
    '#tree' => TRUE,
  );
  $form['gmap_markermanager']['gmap_mm_type'] = array(
    '#type' => 'radios',
    '#tree' => FALSE,
    '#required' => TRUE,
    '#options' => array(
      'gmap' => t('No manager (use GMap API directly)'),
      'gmarkermanager' => t("Google's GMarkerManager"),
      'clusterer' => t("Jef Poskanzer's Clusterer"),
    ),
    '#default_value' => variable_get('gmap_mm_type', 'gmap'),
    '#description' => t('If you are planning on using many markers on a single map, you may want to consider using a marker manager to speed up map rendering.'),
  );

  $form['gmap_markermanager']['gmap'] = array(
    '#type' => 'fieldset',
    '#title' => t('Unmanaged marker settings'),
    '#description' => t('There are no settings for unmanaged markers.'),
  );

  $form['gmap_markermanager']['gmarkermanager'] = array(
    '#type' => 'fieldset',
    '#title' => t('GMarkerManager settings'),
    '#description' => t('GMarkerManager is a new part of the official Google Maps API that provides a marker manager.'),
  );

  $form['gmap_markermanager']['gmarkermanager']['borderPadding'] = array(
    '#type' => 'textfield',
    '#title' => t('Border padding'),
    '#description' => t('Markers located less than this number of pixels from the viewport will be added to the map by the manager (even if they would be fully invisible.)'),
    '#size' => 3,
    '#maxlength' => 3,
    '#default_value' => $opts['gmarkermanager']['borderPadding'],
  );
  $form['gmap_markermanager']['gmarkermanager']['maxZoom'] = array(
    '#type' => 'select',
    '#title' => t('Maximum zoom'),
    '#options' => drupal_map_assoc(range(0, 17)),
    '#description' => t('At the specified zoom level and above, the marker manager will disable itself for additional speed.'),
    '#default_value' => $opts['gmarkermanager']['maxZoom'],
  );
  $form['gmap_markermanager']['gmarkermanager']['trackMarkers'] = array(
    '#type' => 'checkbox',
    '#title' => t('Track markers'),
    '#description' => t('If enabled, the marker manager will track marker movements. Leave off unless you need to move markers around with setPoint.'),
    '#default_value' => $opts['gmarkermanager']['trackMarkers'],
  );
  $form['gmap_markermanager']['gmarkermanager']['defaults'] = array(
    '#type' => 'fieldset',
    '#title' => t('Marker defaults'),
    '#description' => t('Default marker-specific settings for GMarkerManager. Markers will appear when the current zoom level is between minZoom and maxZoom.'),
  );
  $form['gmap_markermanager']['gmarkermanager']['defaults']['markerMinZoom'] = array(
    '#type' => 'select',
    '#title' => t('Minimum zoom'),
    '#options' => drupal_map_assoc(range(0, 17)),
    '#description' => t('At the specified zoom level and above (i.e. zooming in,) the marker will be shown. Choose 0 to show markers at all zoom levels by default.'),
    '#default_value' => $opts['gmarkermanager']['markerMinZoom'],
    '#parents' => array('gmap_markermanager', 'gmarkermanager', 'markerMinZoom'),
  );

  $form['gmap_markermanager']['gmarkermanager']['defaults']['markerMaxZoom'] = array(
    '#type' => 'select',
    '#title' => t('Maximum zoom'),
    '#options' => drupal_map_assoc(range(0, 17)),
    '#description' => t('At the specified zoom level and above (i.e. zooming in,) the marker will be hidden. Choose 0 to disable by default.'),
    '#default_value' => $opts['gmarkermanager']['markerMaxZoom'],
    '#parents' => array('gmap_markermanager', 'gmarkermanager', 'markerMaxZoom'),
  );

  $form['gmap_markermanager']['clusterer'] = array(
    '#type' => 'fieldset',
    '#title' => t('Clusterer settings'),
    '#description' => t("Clusterer is a marker manager written by Jef Poskanzer of acme.com. To use, you must place Clusterer2.js (available ".'<a href="@url">here</a>) into the "thirdparty" folder.', array('@url' => 'http://acme.com/javascript/Clusterer2.js')),
  );
  $form['gmap_markermanager']['clusterer']['clusterer_file'] = array(
    '#type' => 'textfield',
    '#title' => t('Clusterer filename'),
    '#description' => t('Set the name of the Clusterer file in the thirdparty folder.'),
    '#default_value' => $opts['clusterer']['clusterer_file'],
  );
  $form['gmap_markermanager']['clusterer']['marker'] = array(
    '#type' => 'gmap_markerchooser',
    '#title' => t('Marker for clusters'),
    '#description' => t('The marker to use when creating a cluster.'),
    '#default_value' => $opts['clusterer']['marker'],
  );
  $form['gmap_markermanager']['clusterer']['max_nocluster'] = array(
    '#type' => 'textfield',
    '#title' => t('Activate on'),
    '#field_suffix' => t('or more markers'),
    '#description' => t("Clustering is enabled when more than the specified number of markers are visible at the same time."),
    '#size' => 3,
    '#maxlength' => 3,
    '#default_value' => $opts['clusterer']['max_nocluster'],
  );
  $form['gmap_markermanager']['clusterer']['cluster_min'] = array(
    '#type' => 'textfield',
    '#title' => t('Cluster on'),
    '#field_suffix' => t('or more markers'),
    '#description' => t("Minimal number of markers per cluster"),
    '#size' => 2,
    '#maxlength' => 2,
    '#default_value' => $opts['clusterer']['cluster_min'],
  );
  $form['gmap_markermanager']['clusterer']['max_lines'] = array(
    '#type' => 'textfield',
    '#title' => t('Lines per box'),
    '#field_prefix' => t('at most'),
    '#field_suffix' => t('lines'),
    '#description' => t("Maximum number of lines per info box"),
    '#size' => 2,
    '#maxlength' => 2,
    '#default_value' => $opts['clusterer']['max_lines'],
  );

  $form['#validate']['gmap_admin_settings_validate'] = array();

  return system_settings_form($form);
}

function gmap_admin_settings_validate($form_id, $form_values, $form) {
  if ($form_values['op'] == t('Regenerate')) {
    gmap_regenerate_markers();
    drupal_set_message(t('Marker cache regenerated.'));
  }

  $found = FALSE;
  foreach ($form_values['gmap_default']['baselayers'] as $l) {
    if ($l) {
      $found = TRUE;
      break;
    }
  }
  if (!$found) {
    form_set_error('gmap_default][baselayers', t('You must select at least one baselayer!'));
  }

  // Check that the default map type is sane.
  if (!$form_values['gmap_default']['baselayers'][$form_values['gmap_default']['maptype']]) {
    form_error($form['gmap_default']['baselayers']['maptype'], t('The default map type must be an enabled baselayer!'));
  }
}
