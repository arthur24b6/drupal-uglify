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
   * Excluded files.
   *
   * @var array
   */
  var $excluded = array();

  /**
   * Path to the uglifyjs binary.
   * @var string
   */
  var $pathToUglify = FALSE;

  function __construct($file = FALSE) {
    if ($file) {
      $this->minify($file);
    }
    $this->files = variable_get('uglify_files', array());
    $this->excluded =  variable_get('uglify_files_exclude', array());
    $this->uglifyInstalled();
  }

  private function uglifyInstalled() {
    $uglify = variable_get('uglify_path_to_binary', "../node_modules/uglify-js/bin/uglifyjs");
    if (is_executable($uglify)) {
      $this->pathToUglfiy = $uglify;
    }
  }

  /**
   * Generates the complete list of Javascripts to minify.
   */
  public function buildFileList() {
    // @TODO exclude public://
    $javascripts = file_scan_directory(DRUPAL_ROOT , '/.*(?<default/files)(?<!min)\.js$/i');
    foreach ($javascripts as $src => $javscript) {
      if (empty($this->excluded[$src])) {
        $this->fileAdd($src);
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
        'minified' => $this->minfiedFileURI($src),
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
   * Get the registered file array.
   *
   * @param string $path
   *   Registered file array for this path.
   */
  private function getRegisteredFile($path) {
    if (!empty($this->files[$path])) {
      return $this->files[$path];
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
      if (empty($this->files[$src])) {
        $this->addFile($src);
      }
      return $output_file;
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
      if (file_exists($this->files['soruce']['minified'])) {
        return $this->files['soruce']['minified'];
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
