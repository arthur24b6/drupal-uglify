# Uglify JS Minification

## Requirements

* [uglifyjs2](https://github.com/mishoo/UglifyJS2)

## Usage

Enable the module. Browse the Drupal site to collect the in use javascript files. Minified files will be generated on cron runs. The minified files will replace the non-minified ones non-destructively.

* ```drush uglify``` will generate minified files from the known javascript files.
* ```drush uglify-list``` will list all files that uglify currently knows about to minify.

## Configuration

* ```$conf['uglify_use_minified_js']``` Defaults to true. Enables the use of minified files if present. Use this to disable minification during development for example.
* ```$conf['uglify_generate_on_cron']``` Defaults to true. Generates the minified files on Drupal cron runs.
* ```$conf['uglify_path_to_binary']``` Path to uglifyjs. Defaults to the directory above the project root.
* ```$conf['uglify_output_directory']``` Defaults to public://uglifyjs. Where the minified files are stored.

## Todo

* Ensure that the path to uglifyjs is valid
* Licensing concerns.
* Provide a way to identify some files as not minifyiable.
* Scan for javascript files in /misc and /sites/all/modules
