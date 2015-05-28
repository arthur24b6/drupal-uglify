<?php

/**
 * @file
 * Provides the uglify class.
 */

class uglify {

  var $files = array();

  var $pathToUglify = FALSE;

  function __construct($file = FALSE) {
    if ($file) {
      $this->minify($file);
    }
    $this->files = variable_get('uglify_files', array());
    $this->uglifyInstalled();
  }

  function uglifyInstalled() {
    $uglify = variable_get('uglify_path_to_binary', "../node_modules/uglify-js/bin/uglifyjs");
    if (is_executable($uglify)) {
      $this->pathToUglfiy = $uglify;
    }
  }

  /**
   * Utility function to add files to the internal store.
   */
  public function fileAdd($src) {
    if (empty($this->files[$src])) {
      $this->files[$src] = $src; //array('src' => $src, 'minified' => $this->minfiedFileURI($src));
      variable_set('uglify_files', $this->files);
    }
  }

  /**
   * Utility function to remove files from the internal store.
   */
  public function fileDelete($src) {
    if (!empty($this->files[$src])) {
      $path = $this->minfiedFileURI($src);
      if (file_exists($path)) {
        file_unmanaged_delete($path);
      }
    }
  }

  public function minify($src) {
    if (!$this->isMinified($src)) {
      if (!file_exists($this->minfiedFileURI($src))) {
        $this->fileAdd($src);
        $this->minifyFile($src);
      }
    }
  }

  public function minifyFiles() {
    foreach ($this->files as $src => $file) {
      $this->minify($src);
    }
  }

  /**
   * @param string $file
   *   File path relative to the Drupal root to be minified.
   * @param string $output_file
   *   Optional, URI to put the completed file.
   *
   * @return mixed
   *   URI to the output file or false.
   */
  public function minifyFile($file, $output_file = FALSE) {
    $output_file =  drupal_realpath($this->minfiedFileURI($file, $output_file));
    // Ensure that the minfied directory exists- required for drupal_realpath() to work.
    $directory = dirname($this->minfiedFileURI($file, $output_file));
    file_prepare_directory($directory, FILE_CREATE_DIRECTORY);

    $command = $this->pathToUglfiy . ' ' . DRUPAL_ROOT . "/$file -o $output_file";
    if (exec($command)) {
      return $output_file;
    }

  }

  /**
   * Check to see if this file is already minified.
   */
  public function isMinified($file) {
    return preg_match('/.min.js/i', $file);
  }

  /**
   * Creates the minified file base name.
   *
   * @param string $file
   *   Path to file.
   * @param string $output_file
   *   Optional, if present is returned with no modifications.
   *
   * @return string
   *   URI to minified file
   */
  public function minfiedFileURI($file, $output_file = FALSE) {
    if (!$output_file) {
      $minified = preg_replace('/(.*)\.js$/', '$1.min.js', $file);
      $output_file = variable_get('uglify_output_directory', 'public://uglify_js/') . basename($minified);
    }
    return $output_file;
  }

  public function getMinifiedFile($src) {
    $path = $this->minfiedFileURI($src);
    if (file_exists($path)) {
      return $path;
    }
    // Add this file for generation.
    elseif (empty($this->files[$src])) {
      $this->addFile($src);
    }
  }

  /**
   * Attempts to find a file header and insert it into the minified file.
   *
   * @param string $file
   *   URI to file to be minified.
   *
   * @return string
   *   Licence text if found.
   */
  public function findLicence($file) {
// Probably can ignore anything in sites/all/modules/contrib
// Anything in sites/all/libraries should be checked
// Check everything in sites/all/modules/custom
  }




}