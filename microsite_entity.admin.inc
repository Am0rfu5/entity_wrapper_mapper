<?php
/**
 * @file
 * Forms processing for microsites.
 */

/**
 * Generates the task type editing form.
 */
function microsite_entity_type_form($form, &$form_state, $task_type, $op = 'edit') {

  if ($op == 'clone') {
    $task_type->label .= ' (cloned)';
    $task_type->type = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $task_type->label,
    '#description' => t('The human-readable name of this task type.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  // Machine-readable type name.
  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($task_type->type) ? $task_type->type : '',
    '#maxlength' => 32,
    '#disabled' => $task_type->isLocked() && $op != 'clone',
    '#machine_name' => array(
      'exists' => 'microsite_entity_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this task type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#default_value' => isset($task_type->description) ? $task_type->description : '',
    '#description' => t('Description of the microsite type.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save microsite type'),
    '#weight' => 40,
  );

  if (!$task_type->isLocked() && $op != 'add' && $op != 'clone') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete microsite entity'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('microsite_entity_type_form_submit_delete'),
    );
  }
  return $form;
}

/**
 * Submit handler for creating/editing microsite types.
 */
function microsite_entity_type_form_submit(&$form, &$form_state) {
  $task_type = entity_ui_form_submit_build_entity($form, $form_state);
  // Save and go back.
  microsite_entity_type_save($task_type);

  // Redirect user back to list of microsite types.
  $form_state['redirect'] = 'admin/structure/microsite-types';
}

/**
 * Submit handler for deleting microsite types.
 */
function microsite_entity_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/microsite-types/' . $form_state['t_microsite_entity_type']->type . '/delete';
}

/**
 * Microsite type delete form.
 */
function microsite_entity_type_form_delete_confirm($form, &$form_state, $task_type) {
  $form_state['microsite_entity_type'] = $task_type;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['microsite_entity_type_id']
    = array(
      '#type' => 'value',
      '#value' => entity_id('microsite_entity_type', $task_type),
    );
  return confirm_form($form,
    t('Are you sure you want to delete microsite type %title?', array('%title' => entity_label('task_type', $task_type))),
    'microsite/' . entity_id('microsite_entity_type', $task_type),
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Task type delete form submit handler.
 */
function microsite_entity_type_form_delete_confirm_submit($form, &$form_state) {
  $task_type = $form_state['microsite_entity_type'];
  microsite_entity_type_delete($task_type);

  watchdog('microsite_entity_type', '@type: deleted %title.',
    array('@type' => $task_type->type, '%title' => $task_type->label));
  drupal_set_message(t('@type %title has been deleted.',
    array('@type' => $task_type->type, '%title' => $task_type->label)));

  $form_state['redirect'] = 'admin/structure/microsite-types';
}

/**
 * Page to select task Type to add new task.
 */
function microsite_entity_admin_add_page() {
  $items = array();
  foreach (microsite_entity_types() as $task_type_key => $task_type) {
    $items[] = l(entity_label('microsite_entity_type', $task_type), 'microsite/add/' . $task_type_key);
  }
  return array(
    'list' => array(
      '#theme' => 'item_list',
      '#items' => $items,
      '#title' => t('Select type of microsite to create.'),
    ),
  );
}

/**
 * Add new task page callback.
 */
function microsite_entity_add($type) {
  $task_type = microsite_entity_types($type);

  $task = entity_create('microsite_entity', array('type' => $type));
  drupal_set_title(t('Create @name', array('@name' => entity_label('microsite_entity_type', $task_type))));

  $output = drupal_get_form('microsite_entity_form', $task);

  return $output;
}

/**
 * Task Form.
 */
function microsite_entity_form($form, &$form_state, $task) {
  $form_state['microsite'] = $task;

  $form['title'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Name'),
    '#default_value' => $task->title,
  );
  // Don't allow the Microsite title to be changed once it's been created.
  if (!empty($task->title)) {
    $form['title']['#disabled'] = TRUE;
    $form['title']['#description'] = t('Once a microsite has been created it may not be renamed');
  }

  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#default_value' => $task->description,
  );

  $form['uid'] = array(
    '#type' => 'value',
    '#value' => $task->uid,
  );

  field_attach_form('microsite_entity', $task, $form, $form_state);

  $submit = array();
  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions'] = array(
    '#weight' => 100,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save microsite'),
    '#submit' => $submit + array('microsite_entity_form_submit'),
  );

  // Show Delete button if we edit task.
  $task_id = entity_id('microsite_entity', $task);
  if (!empty($task_id) && microsite_entity_access('edit', $task)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('microsite_entity_form_submit_delete'),
    );
  }

  $form['#validate'][] = 'microsite_entity_form_validate';

  return $form;
}

/**
 * Validate microsite submission.
 *
 * A stub.
 */
function microsite_entity_form_validate($form, &$form_state) {
}

/**
 * Microsite submit handler.
 */
function microsite_entity_form_submit($form, &$form_state) {
  $task = $form_state['microsite'];

  entity_form_submit_build_entity('microsite_entity', $task, $form, $form_state);

  if (microsite_entity_save($task)) {

    $task_uri = entity_uri('microsite_entity', $task);

    $form_state['redirect'] = $task_uri['path'];

    drupal_set_message(t('Microsite %title saved.', array('%title' => entity_label('microsite_entity', $task))));
  }
}

/**
 * Microsite submit handler for microsite entity deletion from entity form.
 */
function microsite_entity_form_submit_delete($form, &$form_state) {
  $task = $form_state['microsite'];
  $task_uri = entity_uri('microsite_entity', $task);
  $form_state['redirect'] = $task_uri['path'] . '/delete';
}

/**
 * Delete confirmation form.
 */
function microsite_entity_delete_form($form, &$form_state, $task) {
  $form_state['microsite'] = $task;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['task_type_id']
    = array(
      '#type' => 'value',
      '#value' => entity_id('microsite_entity', $task),
    );
  $task_uri = entity_uri('microsite_entity', $task);
  return confirm_form($form,
    t('Are you sure you want to delete microsite %title?',
      array('%title' => entity_label('microsite_entity', $task))
    ),
    $task_uri['path'],
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Delete form submit handler.
 */
function microsite_entity_delete_form_submit($form, &$form_state) {
  $task = $form_state['microsite'];
  microsite_entity_delete($task);

  drupal_set_message(t('Microsite %title deleted.', array('%title' => entity_label('microsite_entity', $task))));

  $form_state['redirect'] = '<front>';
}
