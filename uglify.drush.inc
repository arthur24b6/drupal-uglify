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
    'description' => 'Generate minfied JS files from registered files.',
    'examples' => array(
      'drush uglify' => 'Minfies all registered JS files. Assumes that the registry has already been built.',
      'drush uglify --file=misc/jquery.js' => 'Minfies the jquery file.',
      'drush uglify --register --files=misc' => 'Register and minify found files. Can be used in conjunction with --files. In this example it will only register files found in the misc directory. By default the entire Drupal root is scanned.',
    ),
    'options' => array(
      'file' => 'Provide a specific file path to register and minify the file.',
      'files' => 'Provide a path to a directory to register and minify files.',
      'register' => 'Register found files',
    )
  );
  $items['uglify-purge'] = array(
    'description' => 'Delete and unregister all files created by uglify.',
    'examples' => array(
      'drush uglify-purge' => 'Removes all files.',
      'drush uglify-purge --file=misc/jquery.js' => 'Purges the minfied jquery file.',
    ),
    'options' => array(
      'file' => 'Provide a specific file path to purge.'
    )
  );
  $items['uglify-uncache'] = array(
    'description' => 'Delete all registered minified files but maintains the file in the registry.',
    'examples' => array(
      'drush uglify-uncache' => 'Removes all files.',
      'drush uglify-uncache --file=misc/jquery.js' => 'Remvoves the minfied jquery file.',
    ),
    'options' => array(
      'file' => 'Provide a specific file path to minify'
    )
  );
  $items['uglify-list'] = array(
    'description' => 'List all files in the registry.',
    'options' => array(
      'excluded' => 'Show excluded files',
    ),
    'examples' => array(
      'drush uglify-list' => 'Show all files.',
      'drush uglify-list --excluded' => 'Show all files excluded from minification',
    ),
  );
  return $items;
}

/**
 * Uglifies registered files.
 */
function drush_uglify() {
  $ugly = new Uglify();
  if ($file = drush_get_options('file', FALSE)) {
    $ugly->minifyFile($file);
    return;
  }
  if (drush_get_options('register', FALSE)) {
    $ugly->buildFileList(drush_get_options('files', FALSE));
  }
  $ugly->minifyFiles();
}

/**
 * Purges and deletes files from the registry.
 */
function drush_uglify_purge() {
  $ugly = new Uglify();
  if ($file = drush_get_option('file', FALSE)) {
    $ugly->purgeFile($file);
  }
  else {
    $ugly->purgeFiles();
  }
}

/**
 * Deletes the minfied files to prepare for regeneration.
 */
function drush_uglify_uncache() {
  $ugly = new Uglify();
  if ($file = drush_get_option('file', FALSE)) {
    $ugly->uncacheFile($file);
  }
  else {
    $ugly->uncacheFiles();
  }
}

/**
 * Displays a list of JS files in the system.
 */
function drush_uglify_list() {
  $ugly = new Uglify();
  if (drush_get_option('excluded', FALSE)) {
    drush_print('Excluded Javascript files:');
    foreach ($ugly->excluded as $excluded) {
      drush_print($excluded);
    }
  }
  else {
    drush_print('Registered Javascript files:');
    foreach ($ugly->files as $file) {
      drush_print("{$file['src']} : {$file['minified']}");
    }
  }
}
