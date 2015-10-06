<?php
/**
 * @file
 * View callback for microsite entities.
 */

/**
 * Microsite view callback.
 */
function microsite_entity_view($microsite_entity) {
  drupal_set_title(entity_label('microsite_entity', $microsite_entity));
  return entity_view('microsite_entity',
    array(entity_id('microsite_entity', $microsite_entity) => $microsite_entity),
    'full');
}
