<?php

/**
 * @file
 * Batch preprocessor. Takes a directory of files and creates binary objects.
 */

/**
 * Sample batch preprocessor.
 */
class IslandoraSampleBatchPreprocessor extends IslandoraBatchPreprocessor {

  /**
   * Constructor method.
   *
   * This is completely unnecessary but bears showing here so we can go through
   * the parameters.
   *
   * @param IslandoraTuque $connection
   *   A connection to Fedora. We're going to pass this on to objects we create
   *   as well; that way we have access to Fedora at all stages of processing.
   * @param array $parameters
   *   Any parameters we want to either use during preprocessing or pass on to
   *   the processing phase. This array is completely undefined; it's up to the
   *   preprocessor and processor to define what parameters they require, and
   *   it's up to the constructor (or some other step along the way) to make
   *   sure that all parameters are present and valid.
   */
  public function __construct(IslandoraTuque $connection, array $parameters) {
    // Let's check and make sure we have the required parameters here. If we
    // don't have them, we should bail.
    if (!isset($parameters['path']) || !isset($parameters['namespace']) || !isset($parameters['parent'])) {
      throw new Exception(t('Missing required parameters for the constructor (requires a path, namespace and parent)'));
    }
    // Otherwise, let's continue.
    parent::__construct($connection, $parameters);
  }

  /**
   * Batch preprocess method.
   *
   * Batch preprocessors require a 'preprocess' method; this method's goal is to
   * generate a series of IslandoraBatchObject objects (or a series of objects
   * extended from this class), and run $this->addToDatabase() on them.
   */
  public function preprocess() {
    // We're just going to scan the directory and preprocess everything inside.
    $files = scandir($this->parameters['path']);
    // We should bail if this failed; there may not be any such directory, or we
    // may not have permission to view it.
    if (!$files) {
      throw new Exception(t('Could not scan the given path.'));
    }
    // Remove the . and .. directories.
    $files = array_diff($files, array('.', '..'));

    // Now, we preprocess.
    foreach ($files as $file) {
      // Manually grab a PID that exists in our given namespace.
      $pid = $this->connection->repository->getNextIdentifier($this->parameters['namespace']);
      // Create a batch object. Realistically, this is just a NewFedoraObject
      // with a few extra methods that generate the object from the third
      // parameter.
      $child = new IslandoraSampleBatchObject($pid, $this->connection->repository, array(
        'path' => "{$this->parameters['path']}/{$file}",
        'parent' => $this->parameters['parent'],
      ));
      // Add this to the database. Specifying the 'parent' allows us to see what
      // the parent is intended to be when viewing the batch queue.
      // The getResources() method is for adding any required resources to the
      // database.
      $this->addToDatabase($child, $child->getResources(), $this->parameters['parent']);
    }
  }
}
