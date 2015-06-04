<?php

/**
 * @file
 * Provides the uglify class.
 */

class uglify {

  /**
   * Registered files.
   *
   * @var array.
   */
  var $files = array();

  /**
   * Excluded files with these paths.
   *
   * @var array
   */
  var $excluded = array();

  /**
   * Path to the uglifyjs binary.
   *
   * @var string
   */
  var $pathToUglify = FALSE;

  function __construct() {

    $this->files = variable_get('uglify_files', array());

    $this->excluded = array(
      variable_get('file_public_path'),
      variable_get('file_private_path'),
      variable_get('file_temporary_path'),
    );
    $this->excluded = array_unique(array_merge(variable_get('uglify_files_exclude', array()), $this->excluded));

    $this->uglifyInstalled();
  }

  private function uglifyInstalled() {
    $uglify = variable_get('uglify_path_to_binary', "/var/www/sites/test.dev/node_modules/uglify-js/bin/uglifyjs");
    if (is_executable($uglify)) {
      $this->pathToUglfiy = $uglify;
    }
  }

  /**
   * Generates the complete list of Javascripts to minify.
   */
  public function buildFileList() {
    $javascripts = file_scan_directory(DRUPAL_ROOT , '/.*(?<!min)\.js$/i');
    foreach ($javascripts as $src => $javscript) {
      // File path is relative to the Drupal root.
      $src = str_replace(DRUPAL_ROOT . '/', '', $src);
      if (!$this->isExcluded($src)) {
        $this->registerFile($src);
      }
    }
  }

  /**
   * Utility function to update the stored file list.
   */
  private function updateFiles() {
    variable_set('uglify_files', $this->files);
  }

  /**
   * Utility function to add files to the internal store.
   */
  public function registerFile($src) {
    if (empty($this->files[$src])) {
      $this->files[$src] = array(
        'src' => $src,
        'minified' => $this->minfiedFileURI($src),
        'modifed' => filemtime($src)
      );
      $this->updateFiles();
    }
  }

  /**
   * Should a file be included in the registry?
   */
  public function isExcluded($src) {
    foreach ($this->excluded as $exclude) {
      if (strstr($src, $exclude)) {
        return TRUE;
      }
    }
  }

  /**
   * Get the registered file array.
   *
   * @param string $src
   *   Registered file array for this path.
   */
  private function getRegisteredFile($src) {
    if (!empty($this->files[$src])) {
      return $this->files[$src];
    }
  }

  /**
   * Should this file be minifed?
   */
  public function shouldUpdate($src, $force = FALSE) {
    if ($force) {
      return TRUE;
    }
    elseif (empty($this->files[$src])) {
      return TRUE;
    }
    elseif (filemtime($src) > $this->files[$src]['modifed']) {
      return TRUE;
    }
  }

  /**
   * Perform the file minification.
   *
   * @param string $src
   *   System file path or URI.
   * @param string $output_file
   *   Optional, URI to put the completed file.
   *
   * @return mixed
   *   URI to the output file or FALSE.
   */
  public function minifyFile($src) {
    $uri = $this->minfiedFileURI($src);
    $directory = dirname($uri);
    // Ensure that the minfied directory exists- required for drupal_realpath() to work.
    if (!file_prepare_directory($directory, FILE_CREATE_DIRECTORY)) {
      watchdog('uglify', "Could not create directory $directory", array(), WATCHDOG_ERROR);
      return FALSE;
    }

    $output_file = drupal_realpath($uri);
    $command = $this->pathToUglfiy . " $src -o $output_file " . variable_get('uglify_comments', '--comments all');

    exec($command, $return, $status);
    if (!$status) {
      if (empty($this->files[$src])) {
        $this->addFile($src);
      }
      return $output_file;
    }
    else {
      watchdog('uglify', "Could not execute: $command", array(), WATCHDOG_ERROR);
    }
  }

  /**
   * Minify all files that are being tracked.
   */
  public function minifyFiles($force = FALSE) {
    foreach ($this->files as $item) {
      $this->minifyFile($item['src'], $force);
    }
  }

  /**
   * Creates the minified file base name.
   *
   * @param string $src
   *   Path to file.
   *
   * @return string
   *   URI to minified file
   */
  public function minfiedFileURI($src) {
    $minified = preg_replace('/(.*)\.js$/', '$1.min.js', $src);
    return variable_get('uglify_output_directory', "public://uglifyjs/") . "$minified";
  }

  /**
   * Get the minified file path for the source file.
   *
   * @param string $src
   *   Path to the source file.
   *
   * @return string
   *   URI to minified file.
   */
  public function getMinified($src) {
    if (!empty($this->files[$src])) {
      if (file_exists($this->files[$src]['minified'])) {
        return $this->files[$src]['minified'];
      }
    }
    return FALSE;
  }

  /**
   * Delete a minified URI.
   *
   * @param string $uri
   *   Path to the minified file to delete.
   */
  public function deleteMinified($uri) {
    file_unmanaged_delete($uri);
  }

  /**
   * Purge a file from the minfied files and the internal data store.
   *
   * @param mixed $item
   *   Registered file uri.
   */
  public function purgeFile($item) {
    if (!is_array($item) && ! $item = $this->getRegisteredFile($item)) {
      return FALSE;
    }
    $this->deleteMinified($item['minified']);
    unset($this->files[$item['src']]);
    $this->updateFiles();
  }

  /**
   * Purge all registered files.
   */
  public function purgeFiles() {
    foreach ($this->files as $item) {
      $this->purgeFile($item);
    }
  }

  /**
   * Delete the minified file and unregister it.
   *
   * @param mixed $item
   *   Array or path to source file.
   */
  public function uncacheFile($item) {
    if (is_string($item)){
      if (! $item = $this->getRegisteredFile($item)) {
        return FALSE;
      }
    }
    $this->deleteMinified($item['minified']);
    $this->files[$item['src']]['modified'] = 0;
    $this->updateFiles();
  }

  /**
   * Uncache all registered files.
   */
  public function uncacheFiles() {
    foreach ($this->files as $item) {
      $this->uncacheFile($item);
    }
  }

  /**
   * Exclude a file or path from the registry.
   *
   * @param string $src
   *   File path or directory path to exclude.
   */
  public function excludeFile($src) {
    if (!in_array($src, $this->excluded)) {
      $this->excluded[] = $src;
      variable_set('uglify_files_exclude', $this->excluded);
    }
    // Remove any matches from the registry.
    foreach ($this->files as $path => $item) {
      if ($this->isExcluded($src)) {
        $this->purgeFile($item);
      }
    }
  }

  /**
   * Remove a file from being excluded.
   *
   * @param string $src
   *   File path or directory path to exclude.
   */
  public function unExcludeFile($src) {
    if ($key = array_search($src, $this->excluded)) {
      unset($this->excluded[$key]);
      variable_set('uglify_files_exclude', $this->excluded);
    }
  }

}
