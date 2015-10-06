<?php
/**
 * @file
 * Hooks provided by this module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Acts on microsite_entity being loaded from the database.
 *
 * This hook is invoked during $microsite_entity loading, which is handled by
 * entity_load(), via the EntityCRUDController.
 *
 * @param array $entities
 *   An array of $microsite_entity entities being loaded, keyed by id.
 *
 * @see hook_entity_load()
 */
function hook_microsite_entity_load(array $entities) {
  $result = db_query('SELECT pid, foo FROM {mytable} WHERE pid IN(:ids)', array(':ids' => array_keys($entities)));
  foreach ($result as $record) {
    $entities[$record->pid]->foo = $record->foo;
  }
}

/**
 * Responds when a $microsite_entity is inserted.
 *
 * This hook is invoked after the $microsite_entity is inserted into the
 * database.
 *
 * @param MicrositeEntity $microsite_entity
 *   The $microsite_entity that is being inserted.
 *
 * @see hook_entity_insert()
 */
function hook_microsite_entity_insert(MicrositeEntity $microsite_entity) {
  db_insert('mytable')
    ->fields(array(
      'id' => entity_id('microsite_entity', $microsite_entity),
      'extra' => print_r($microsite_entity, TRUE),
    ))
    ->execute();
}

/**
 * Acts on a $microsite_entity being inserted or updated.
 *
 * This hook is invoked before the $microsite_entity is saved to the database.
 *
 * @param MicrositeEntity $microsite_entity
 *   The $microsite_entity that is being inserted or updated.
 *
 * @see hook_entity_presave()
 */
function hook_microsite_entity_presave(MicrositeEntity $microsite_entity) {
  $microsite_entity->name = 'foo';
}

/**
 * Responds to a $microsite_entity being updated.
 *
 * This hook is invoked after $microsite_entity has been updated in the
 * database.
 *
 * @param MicrositeEntity $microsite_entity
 *   The $microsite_entity that is being updated.
 *
 * @see hook_entity_update()
 */
function hook_microsite_entity_update(MicrositeEntity $microsite_entity) {
  db_update('mytable')
    ->fields(array('extra' => print_r($microsite_entity, TRUE)))
    ->condition('id', entity_id('microsite_entity', $microsite_entity))
    ->execute();
}

/**
 * Responds to $microsite_entity deletion.
 *
 * This hook is invoked after $microsite_entity has been removed from the
 * database.
 *
 * @param MicrositeEntity $microsite_entity
 *   The $microsite_entity that is being deleted.
 *
 * @see hook_entity_delete()
 */
function hook_microsite_entity_delete(MicrositeEntity $microsite_entity) {
  db_delete('mytable')
    ->condition('pid', entity_id('microsite_entity', $microsite_entity))
    ->execute();
}

/**
 * Act on a microsite_entity that is being assembled before rendering.
 *
 * @param object $microsite_entity
 *   The microsite_entity entity.
 * @param string $view_mode
 *   The view mode the microsite_entity is rendered in.
 * @param string $langcode
 *   The language code used for rendering.
 *
 * The module may add elements to $microsite_entity->content prior to rendering.
 * The structure of $microsite_entity->content is a renderable array as expected
 * by drupal_render().
 *
 * @see hook_entity_prepare_view()
 * @see hook_entity_view()
 */
function hook_microsite_entity_view($microsite_entity, $view_mode, $langcode) {
  $microsite_entity->content['my_additional_field'] = array(
    '#markup' => $additional_field,
    '#weight' => 10,
    '#theme' => 'mymodule_my_additional_field',
  );
}

/**
 * Alter the results of entity_view() for microsite_entity.
 *
 * @param array $build
 *   A renderable array representing the microsite_entity content.
 *
 * This hook is called after the content has been assembled in a structured
 * array and may be used for doing processing which requires that the complete
 * microsite_entity content structure has been built.
 *
 * If the module wishes to act on the rendered HTML of the microsite_entity
 * rather than the structured content array, it may use this hook to add a
 * #post_render callback. Alternatively, it could also implement
 * hook_preprocess_microsite_entity().
 * See drupal_render() and theme() documentation respectively for details.
 *
 * @see hook_entity_view_alter()
 */
function hook_microsite_entity_view_alter($build) {
  if ($build['#view_mode'] == 'full' && isset($build['an_additional_field'])) {
    // Change its weight.
    $build['an_additional_field']['#weight'] = -10;

    // Add a #post_render callback to act on the rendered HTML of the entity.
    $build['#post_render'][] = 'my_module_post_render';
  }
}

/**
 * Acts on microsite_entity_type being loaded from the database.
 *
 * This hook is invoked during microsite_entity_type loading, which is handled
 * by entity_load(), via the EntityCRUDController.
 *
 * @param array $entities
 *   An array of microsite_entity_type entities being loaded, keyed by id.
 *
 * @see hook_entity_load()
 */
function hook_microsite_entity_type_load(array $entities) {
  $result = db_query('SELECT pid, foo FROM {mytable} WHERE pid IN(:ids)', array(':ids' => array_keys($entities)));
  foreach ($result as $record) {
    $entities[$record->pid]->foo = $record->foo;
  }
}

/**
 * Responds when a microsite_entity_type is inserted.
 *
 * This hook is invoked after the microsite_entity_type is inserted into the
 * database.
 *
 * @param MicrositeEntityType $microsite_entity_type
 *   The microsite_entity_type that is being inserted.
 *
 * @see hook_entity_insert()
 */
function hook_microsite_entity_type_insert(MicrositeEntityType $microsite_entity_type) {
  db_insert('mytable')
    ->fields(array(
      'id' => entity_id('microsite_entity_type', $microsite_entity_type),
      'extra' => print_r($microsite_entity_type, TRUE),
    ))
    ->execute();
}

/**
 * Acts on a microsite_entity_type being inserted or updated.
 *
 * This hook is invoked before the microsite_entity_type is saved to the
 * database.
 *
 * @param MicrositeEntityType $microsite_entity_type
 *   The microsite_entity_type that is being inserted or updated.
 *
 * @see hook_entity_presave()
 */
function hook_microsite_entity_type_presave(MicrositeEntityType $microsite_entity_type) {
  $microsite_entity_type->name = 'foo';
}

/**
 * Responds to a microsite_entity_type being updated.
 *
 * This hook is invoked after the microsite_entity_type has been updated in the
 * database.
 *
 * @param MicrositeEntityType $microsite_entity_type
 *   The microsite_entity_type that is being updated.
 *
 * @see hook_entity_update()
 */
function hook_microsite_entity_type_update(MicrositeEntityType $microsite_entity_type) {
  db_update('mytable')
    ->fields(array('extra' => print_r($microsite_entity_type, TRUE)))
    ->condition('id', entity_id('microsite_entity_type', $microsite_entity_type))
    ->execute();
}

/**
 * Responds to microsite_entity_type deletion.
 *
 * This hook is invoked after the microsite_entity_type has been removed
 * from the database.
 *
 * @param MicrositeEntityType $microsite_entity_type
 *   The microsite_entity_type that is being deleted.
 *
 * @see hook_entity_delete()
 */
function hook_microsite_entity_type_delete(MicrositeEntityType $microsite_entity_type) {
  db_delete('mytable')
    ->condition('pid', entity_id('microsite_entity_type', $microsite_entity_type))
    ->execute();
}

/**
 * Define default microsite_entity_type configurations.
 *
 * @return array
 *   An array of default microsite_entity_type, keyed by machine names.
 *
 * @see hook_default_microsite_entity_type_alter()
 */
function hook_default_microsite_entity_type() {
  $defaults['main'] = entity_create('microsite_entity_type', array(
    // …
  ));
  return $defaults;
}

/**
 * Alter default microsite_entity_type configurations.
 *
 * @param array $defaults
 *   An array of default microsite_entity_type, keyed by machine names.
 *
 * @see hook_default_microsite_entity_type()
 */
function hook_default_microsite_entity_type_alter(array &$defaults) {
  $defaults['main']->name = 'custom name';
}
