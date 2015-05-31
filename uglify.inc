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
   * Generates the complete list of Javascripts to minify.
   */
  function getFileList() {
    $exclude = variable_get('uglify_files_exclude', array());    
    $javascripts = file_scan_directory(DRUPAL_ROOT , '/.*(?<!min)\.js$/i');
    foreach ($javascripts as $src => $javscript) {
      if (empty($exclude[$src])) {
        $ugly->fileAdd($src);
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
  public function fileAdd($src) {
    if (empty($this->files[$src])) {
      $this->files[$src] = array(
        'src' => $src,
        'minified' => $this->minfiedFileURI($src)
        'modifed' => filemtime($src)
      );
      $this->updateFiles();
    }
  }

  /**
   * Utility function to remove files from the internal store.
   */
  public function fileDelete($src) {
    if (!empty($this->files[$src])) {
      if (file_exists($this->files[$src]['minified'])) {
        file_unmanaged_delete($this->files[$src]['minified']);
      }
      unset($this->files[$src]);
      $this->updateFiles();
    }
  }

  /**
   * Should this file be minifed?
   */
  public function shouldUpdate($src, $force = FALSE) {
    if ($force) {
      return TRUE:
    }
    elseif (empty($this->files[$src])) {
      return TRUE;
    }
    elseif (filemtime($src) > $this->files[$src]['modifed']) {
      return TRUE;
    }
  }

  /**
   * Minify a file.
   *
   * @param string $src
   *   Path to the source file.
   * @param bool $force
   *   Optional, re-minify the file regardless.
   */
  public function minify($src, $force = FALSE) {
    if ($this->shouldUpdate($src, $force)) {
      $this->fileAdd($src);
      $this->minifyFile($src);
    }
  }

  /**
   * Minify all files that are being tracked.
   */
  public function minifyFiles($force = FALSE) {
    foreach ($this->files as $src => $file) {
      $this->minify($src, $force);
    }
  }

  /**
   * @param string $src
   *   System file path or URI.
   * @param string $output_file
   *   Optional, URI to put the completed file.
   *
   * @return mixed
   *   URI to the output file or false.
   */
  public function minifyFile($src, $output_file = FALSE) {
    $output_file =  drupal_realpath($this->minfiedFileURI($src, $output_file));
    // Ensure that the minfied directory exists- required for drupal_realpath() to work.
    $directory = dirname($this->minfiedFileURI($src, $output_file));
    file_prepare_directory($directory, FILE_CREATE_DIRECTORY);
    $command = $this->pathToUglfiy . " $src -o $output_file";
    if (exec($command)) {
      return $output_file;
    }
  }

  /**
   * Creates the minified file base name.
   *
   * @param string $src
   *   Path to file.
   * @param string $output_file
   *   Optional, if present is returned with no modifications.
   *
   * @return string
   *   URI to minified file
   */
  public function minfiedFileURI($src, $output_file = FALSE) {
    if (!$output_file) {
      $minified = preg_replace('/(.*)\.js$/', '$1.min.js', $src);
      $output_file = variable_get('uglify_output_directory', 'public://uglifyjs/') . basename($minified);
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
