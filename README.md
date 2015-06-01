# Uglify JS Minification

## About

Provides minification services for Drupal.

## Requirements

* [uglifyjs2](https://github.com/mishoo/UglifyJS2)

## Usage

Enable the module. Build the minification registry and minify all javascript files by doing:
```drush uglify --build```

Once the registry is build and files are minified, Drupal will automatically replace registered javascript files with their minified subsitute.

You can see what files are in the minification registry with:
```drush uglify-list```

## Configuration

* ```$conf['uglify_use_minified_js']``` Defaults to true. Enables the use of minified files if present. Use this to disable minification during development for example.
* ```$conf['uglify_files_exclude']``` Absolute file paths or directories to exclude files from the minified registry. Drupal's public, private, and temporary directories are always excluded.
* ```$conf['uglify_path_to_binary']``` Path to uglifyjs. Defaults to the directory above the project root.
* ```$conf['uglify_output_directory']``` Defaults to public://uglifyjs. Where the minified files are stored.

## Todo

* Ensure that the path to uglifyjs is valid
* Licensing concerns. This is a big one.
* Tests
* Individual file updates to the registry
