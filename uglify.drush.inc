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
      'drush uglify' => 'Minfies all registered JS files.',
      'drush uglify --file=misc/jquery.js' => 'Minfies the jquery file.',
      'drush uglify --force' => 'Regenates all minified files regardless of modified date',
      'drush uglify --build' => 'Find and register all javascript files and then build. '
    ),
    'options' => array(
      'file' => 'Provide a specific file path to minify.',
      'build' => 'Build list of Javascript files to minify and then minify.'
    )
  );
  $items['uglify-purge'] = array(
    'description' => 'Delete and unregister all files created by uglify.
      Generally you want to use uglify-uncache, however this can be handy when a
      file is throwing errors due to minification or other instances.',
    'examples' => array(
      'drush uglify-purge' => 'Removes all files.',
      'drush uglify-purge --file=misc/jquery.js' => 'Purges the minfied jquery file.',
    ),
    'options' => array(
      'file' => 'Provide a specific file path to minify'
    )
  );
  $items['uglify-uncache'] = array(
    'description' => 'Delete all registered minified files. Allows for files to be rebuilt.',
    'examples' => array(
      'drush uglify-uncache' => 'Removes all files.',
      'drush uglify-uncache --file=misc/jquery.js' => 'Remvoves the minfied jquery file.',
    ),
    'options' => array(
      'file' => 'Provide a specific file path to minify'
    )
  );
  $items['uglify-list'] = array(
    'description' => 'List all files that can be minfied.',
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
  $force = drush_get_option('force', FALSE);
  if ($file = drush_get_option('file', FALSE)) {
    $ugly->minifyFile($file, $force);
  }
  else {
    if (drush_get_option('build', FALSE)) {
      $ugly->buildFileList();
    }
    $ugly->minifyFiles($force);
  }
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
