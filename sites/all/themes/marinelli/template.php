<?php
//template for Marinelli Theme
//author: singalkuppe - www.signalkuppe.com

// regions for marinelli
function marinelli_regions() {
    return array(
        'sidebar_left' => t('sidebar_left'),
        'sidebar_right' => t('sidebar_right'),
        'content' => t('content'),
        'footer' => t('footer'),
    );
}


function marinelli_width($left, $right) {
  $width = 540;
  if (!$left ) {
    $width = $width +190;
  }  
  
   if (!$right) {
    $width = $width +190;
  }
  return $width;
}

function _phptemplate_variables($hook, $vars = array()) {
    switch ($hook) {
      case 'page':
        global $theme;      
        drupal_add_css($vars['directory'] .'/additional.css', 'theme', 'all');        

        drupal_add_js($vars['directory'] .'/jquery.color.js','theme');
        drupal_add_js($vars['directory'] .'/scripts.js','theme');

        // turn off title?
        $vars['suppress_title'] = ($vars['node']->type=='crime');

        break;
    }
    
    $vars['css'] = drupal_add_css();
    $vars['styles'] = drupal_get_css();
    $vars['scripts'] = drupal_get_js();
    

    
    return $vars;
}

/**
 * Return a themed breadcrumb trail.
 *
 * @param $breadcrumb
 *   An array containing the breadcrumb links.
 * @return a string containing the breadcrumb output.
 */
function phptemplate_breadcrumb($breadcrumb) {
  if (!empty($breadcrumb)) {
      $breadcrumb[] = drupal_get_title();
        array_shift($breadcrumb);
       return '<div class="path"><p><!--<span>'.t('You are here').'</span>-->'. implode(' / ', $breadcrumb) .'</p></div>';
  }
  }
