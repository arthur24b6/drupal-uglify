<?php

/**
 * @file
 * Provides Drush integration for uglification
 */

/**
 * Implements hook_drush_command().
 */
function uglify_drush_command() {
  $items['uglify'] = array(
    'description' => 'Generate minfied JS files.',
    'examples' => array(
      'drush uglify' => 'Minfies all JS files that have been found.',
    ),
  );
  $items['uglify-list'] = array(
    'description' => 'List all files that can be minfied.',
    'examples' => array(
      'drush uglify-list' => 'Show all files.',
    ),
  );
  return $items;
}

/**
 * Uglifies all known JS files.
 */
function drush_uglify() {
  $ugly = new Uglify();
  $ugly->minifyFiles();
}

/**
 * Displays a list of JS files in the system.
 */
function drush_uglify_list() {
  $ugly = new Uglify();
  drush_print('Javascript files that have been registered:');
  foreach ($ugly->files as $file) {
    drush_print($file);
  }
}