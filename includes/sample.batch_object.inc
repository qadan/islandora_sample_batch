<?php

/**
 * @file
 * Sample batch object. Will be generating binary objects.
 */

/**
 * Batch object class.
 */
class IslandoraSampleBatchObject extends IslandoraBatchObject {

  // Parameters for this batch object.
  protected $parameters;

  /**
   * Constructor.
   *
   * This includes the setting of 'parameters' passed in, so we know where the
   * path to the file is.
   *
   * @param string $pid
   *   The PID to give this object.
   * @param array $parameters
   *   An array of parameters to use to construct this object, including the
   *   path to the binary file to use, as well as the PID of the parent.
   */
  public function __construct($pid, FedoraRepository $repository, array $parameters) {
    parent::__construct($pid, $repository);
    $this->parameters = $parameters;
  }

  /**
   * Processes this item.
   *
   * Standard practice is to not place too much in the batch preprocessor
   * itself; it's much cleaner to sort everything into a series of things you
   * plan to do, and do them all in their own methods.
   */
  public function batchProcess() {
    $this->addRelationships();
    // Attempt to add the OBJ. A couple things can go wrong there; if anything
    // does, return the error state.
    try {
      $added = $this->addOBJ();
    }
    catch (Exception $e) {
      return ISLANDORA_BATCH_STATE__ERROR;
    }
    // In the end, we return the batch state. We should return __DONE if nothing
    // went wrong, __ERROR if anything did. A few other options exists, but are
    // not typically used.
    return $added ? ISLANDORA_BATCH_STATE__DONE : ISLANDORA_BATCH_STATE_ERROR;
  }

  /**
   * Adds the model and parent.
   */
  public function addRelationships() {
    // Models can be accessed in the 'models' array, rather than manually
    // setting relationships.
    $this->models = 'islandora:binaryObjectCModel';
    // Parents have to be manually added using the 'relationships' object.
    $this->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOfCollection', $this->parameters['parent']);
  }

  /**
   * No resources to get, so no-op for this one.
   */
  public function getResources() {
    return array();
  }

  /**
   * Adds the file as an OBJ datastream.
   *
   * @return bool
   *   Whether or not the datastream ingest was successful.
   */
  public function addOBJ() {
    // Create a new datastream.
    $obj = $this->constructDatastream('OBJ', 'M');
    // Set some properties.
    $obj->label = 'OBJ Datastream';
    $obj->mimetype = 'application/octet-stream';
    // Load from file.
    dd($this->parameters['path']);
    $obj->setContentFromFile($this->parameters['path'], TRUE);
    // The process of ingesting can fail; if it does, FALSE is returned.
    return $this->ingestDatastream($obj);
  }
}
