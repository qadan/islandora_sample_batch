<?php

/**
 * @file
 * The part that does the Drush command.
 */

/**
 * Implements hook_drush_command().
 */
function islandora_sample_batch_drush_command() {
  return array(
    'islandora_sample_batch_preprocess' => array(
      'description' => dt('Sample batch.'),
      'drupal dependencies' => array(
        'islandora_batch',
        'islandora_sample_batch',
      ),
      'options' => array(
        // We're going to need to know the path to our objects.
        'path' => array(
          'description' => dt('Path to files.'),
          'example-value' => '/path/to/files',
          'required' => TRUE,
        ),
        // We want to put those objects somewhere, though we default.
        'parent' => array(
          'description' => dt('Parent collection.'),
          'example-value' => 'islandora:root',
          'required' => FALSE,
        ),
        // Since the default namespace for Fedora is probably 'changeme', we
        // will want to be able to specify that.
        'namespace' => array(
          'description' => dt('Namespace.'),
          'example-value' => 'islandora',
          'required' => FALSE,
        ),
      ),
    ),
  );
}

/**
 * Callback for the batch preprocessor.
 */
function drush_islandora_sample_batch_preprocess() {
  // Get the Drush options.
  $path = drush_get_option('path');
  $parent = drush_get_option('parent', 'islandora:root');
  $namespace = drush_get_option('namespace', 'islandora');

  // Get a connection to Fedora.
  // XXX: A connection may be created without credentials in some circumstances;
  // this is due to how Drush's bootstrapping works. Resetting here to ensure a
  // connection with proper credentials gets created.
  drupal_static_reset('islandora_get_tuque_connection');
  $connection = islandora_get_tuque_connection();

  // Create a batch preprocessor and 'handle' it. We pass it our connection to
  // Fedora, as well as a series of parameters we'll use to create the objects.
  $preprocessor = new IslandoraSampleBatchPreprocessor($connection, array(
    'path' => $path,
    'parent' => $parent,
    'namespace' => $namespace,
  ));
  // This is realistically just a plain wrapper on top of the preprocessor's
  // preprocess() method.
  islandora_batch_handle_preprocessor($preprocessor);

  // Log the Set ID so we know which one to check and/or ingest.
  drush_log(t('SetId: @s', array('@s' => $preprocessor->getSetId())), 'ok');
}
