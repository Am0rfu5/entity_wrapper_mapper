<?php
/**
 * @file
 * Provides streamwrappers for Hosted and Integrated Microsites.
 */

/**
 * TI Hosted Microsite (hosted://) stream wrapper class.
 *
 * Provides support for storing privately accessible files with the Drupal file
 * interface.
 */
class MicrositeEntityHostedStreamWrapper extends DrupalLocalStreamWrapper {
  /**
   * Implements abstract public function getDirectoryPath().
   */
  public function getDirectoryPath() {
    return variable_get('microsite_hosted_path', '');
  }

  /**
   * Overrides getExternalUrl().
   *
   * Return the HTML URI of a private file.
   */
  public function getExternalUrl() {
    $hosted_path = variable_get('hosted_url_dir');
    if (!$hosted_path) {
      $hosted_path ='microsites';
    }
    $path = str_replace('\\', '/', $this->getTarget());
    return url($hosted_path . '/' . $path, array('absolute' => TRUE));
  }
}

/**
 * TI Integrated Microsite (integrated://) stream wrapper class.
 *
 * Provides support for storing privately accessible files with the Drupal file
 * interface.
 */
class MicrositeEntityIntegratedStreamWrapper extends DrupalLocalStreamWrapper {
  /**
   * Implements abstract public function getDirectoryPath().
   */
  public function getDirectoryPath() {
    return variable_get('microsite_integrated_path', '');
  }

  /**
   * Overrides getExternalUrl().
   *
   * Return the HTML URI of a private file.
   */
  public function getExternalUrl() {
    $path = str_replace('\\', '/', $this->getTarget());
    return url('integrated/files/' . $path, array('absolute' => TRUE));
  }
}
