<?php
// $Id: content_crud.inc,v 1.4.2.15 2008/10/07 11:56:48 karens Exp $

/**
 * @file
 * Create/Read/Update/Delete functions for CCK-defined object types.
 */

/**
 * Expand field info to create field => widget info.
 */
function content_field_instance_expand($field) {
  if (isset($field['widget'])) {
    return $field;
  }
  $field['widget'] = !empty($field['widget_settings']) ? $field['widget_settings'] : array();
  $field['widget']['label'] = !empty($field['label']) ? $field['label'] : $field['field_name'];
  $field['widget']['weight'] = !empty($field['weight']) ? $field['weight'] : 0;
  $field['widget']['description'] = !empty($field['description']) ? $field['description'] : '';

  if (!empty($field['widget_type'])) {
    $field['widget']['type'] = $field['widget_type'];
    $widget_types = _content_widget_types();
    $field['widget']['module'] = isset($widget_types[$field['widget_type']]['module']) ? $widget_types[$field['widget_type']]['module'] : $field['widget_module'];
  }
  elseif (!empty($field['widget_module'])) {
    $field['widget']['module'] = $field['widget_module'];
  }

  unset($field['widget_type']);
  unset($field['weight']);
  unset($field['label']);
  unset($field['description']);
  unset($field['widget_module']);
  unset($field['widget_settings']);

  // If content.module is handling the default value,
  // initialize $widget_settings with default values,
  if (isset($field['default_value']) && isset($field['default_value_php']) &&
      content_callback('widget', 'default value', $field) == CONTENT_CALLBACK_DEFAULT) {
    $field['widget']['default_value'] = !empty($field['default_value']) ? $field['default_value']  : NULL;
    $field['widget']['default_value_php'] = !empty($field['default_value_php']) ? $field['default_value_php'] : NULL;
    unset($field['default_value']);
    unset($field['default_value_php']);
  }
  return $field;
}

/**
 * Collapse field info from field => widget to flattened form values.
 */
function content_field_instance_collapse($field) {
  if (!isset($field['widget'])) {
    return $field;
  }
  $field['widget_settings'] = !empty($field['widget']) ? $field['widget'] : array();
  $field['widget_type'] = !empty($field['widget']['type']) ? $field['widget']['type'] : '';
  $field['weight'] = !empty($field['widget']['weight']) ? $field['widget']['weight'] : 0;
  $field['label'] = !empty($field['widget']['label']) ? $field['widget']['label'] : $field['field_name'];
  $field['description'] = !empty($field['widget']['description']) ? $field['widget']['description'] : '';
  $field['type_name'] = !empty($field['type_name']) ? $field['type_name'] : '';

  if (!empty($field['widget']['module'])) {
    $widget_module = $field['widget']['module'];
  }
  elseif (!empty($field['widget']['type'])) {
    $widget_types = _content_widget_types();
    $widget_module = $widget_types[$field['widget']['type']]['module'];
  }
  else {
    $widget_module = '';
  }
  $field['widget_module'] = $widget_module;
  unset($field['widget_settings']['type']);
  unset($field['widget_settings']['weight']);
  unset($field['widget_settings']['label']);
  unset($field['widget_settings']['description']);
  unset($field['widget_settings']['module']);
  unset($field['widget']);
  return $field;
}

/**
 * Rebuild content type information from node tables.
 *
 *  Used to update CCK tables that might have missed changes made when CCK was disabled.
 *  Called by hook_form_alter() on system modules page whenever CCK is enabled.
 */
function content_types_rebuild() {
  $db_types = content_types();

  $result = db_query("SELECT type_name FROM {node_field_instance}");
  while ($type = db_fetch_array($result)) {
    $field_types[] = $type['type_name'];
  }

  foreach ($db_types as $content_type) {
    // Find content types that are missing the content table and add it
    content_type_create((object)array('type' => $content_type['type']));
  }
  content_clear_type_cache();
}

/**
 * Make changes needed when a content type is created.
 *
 * @param $info
 *   value supplied by hook_node_type()
 */
function content_type_create($info) {
  content_clear_type_cache();
  $type = content_types($info->type);
  $table = _content_tablename($type['type'], CONTENT_DB_STORAGE_PER_CONTENT_TYPE);
  if (!db_table_exists($table)) {
    switch ($GLOBALS['db_type']) {
      case 'mysql':
      case 'mysqli':
        db_query("CREATE TABLE {". $table ."} (
          vid int unsigned NOT NULL default '0',
          nid int unsigned NOT NULL default '0',
          PRIMARY KEY (vid),
          KEY nid (nid)
        ) /*!40100 DEFAULT CHARACTER SET utf8 */");
      break;

      case 'pgsql':
        db_query("CREATE TABLE {". $table ."} (
          vid int_unsigned NOT NULL default '0',
          nid int_unsigned NOT NULL default '0',
          PRIMARY KEY (vid)
        )");
        db_query("CREATE INDEX {". $table ."}_nid_idx ON {". $table ."}(nid)");
      break;
    }
    drupal_set_message(t('The content fields table %name has been created.', array('%name' => $table)));
  }
}

/**
 * Make changes needed when an existing content type is updated.
 *
 * @param $info
 *   value supplied by hook_node_type()
 */
function content_type_update($info) {
  if (!empty($info->old_type) && $info->old_type != $info->type) {
    // rename the content type in all fields that use changed content type.
    db_query("UPDATE {node_field_instance} SET type_name='%s' WHERE type_name='%s'", array($info->type, $info->old_type));

    // Rename the content fields table to match new content type name.
    $old_type = content_types($info->old_type);
    $old_name = _content_tablename($old_type['type'], CONTENT_DB_STORAGE_PER_CONTENT_TYPE);
    $new_name = _content_tablename($info->type, CONTENT_DB_STORAGE_PER_CONTENT_TYPE);
    if (db_table_exists($old_name)) {
      switch ($GLOBALS['db_type']) {
        case 'mysql':
        case 'mysqli':
          db_query("RENAME TABLE {". $old_name ."} TO {". $new_name ."}");
          break;

        case 'pgsql':
          db_query("ALTER TABLE {". $old_name ."} RENAME TO {". $new_name ."}");
          break;
      }
      drupal_set_message(t('Content fields table %old_name has been renamed to %new_name and field instances have been updated.', array('%old_name' => $old_name, '%new_name' => $new_name)));
    }
  }
  // reset all content type info.
  content_clear_type_cache();
}

/**
 * Make changes needed when a content type is deleted.
 *
 * @param $info
 *   value supplied by hook_node_type()
 */
function content_type_delete($info) {
  // Don't delete data for content-type defined by disabled modules.
  if (!empty($info->disabled)) {
    return;
  }

  $type = content_types($info->type);

  foreach ($type['fields'] as $field) {
    content_field_instance_delete(array('type_name' => $info->type, 'field_name' => $field['field_name']));
  }
  $table = _content_tablename($type['type'], CONTENT_DB_STORAGE_PER_CONTENT_TYPE);
  if (db_table_exists($table)) {
    db_query('DROP TABLE {'. $table .'}');
    drupal_set_message(t('The content fields table %name has been deleted.', array('%name' => $table)));
  }

  // Reset all content type info.
  content_clear_type_cache();
}


/**
 * Create a new field.
 *
 * Any call to this function *must* be immediately followed by a call to
 * content_field_instance_create(), or the database could be left in an
 * inconsistent state.
 *
 * @param $properties
 *   An array of properties to load the field with. Valid keys:
 *   - '' -
 * @return
 *   The ID of the newly-created field.
 */
function content_field_create($properties) {
  // TODO
}

/**
 * Load a field.
 *
 * @param $properties
 *   An array of properties to use in selecting a field. Valid keys:
 *   - '' -
 * @return
 *   The field array.
 */
function content_field_read($properties) {
  // TODO
}

/**
 * Update an existing field.
 *
 * @param $properties
 *   An array of properties to set in the field. Valid keys:
 *   - '' -
 * @return
 *   The number of fields updated.
 */
function content_field_update($properties) {
  // TODO
}

/**
 * Delete an existing field.
 *
 * @param $properties
 *   An array of properties to use in selecting a field. Valid keys:
 *   - 'field_name' - The name of the field to be deleted.
 * @return
 *   The number of fields deleted.
 */
function content_field_delete($properties) {
  $result = db_query("SELECT type_name FROM {node_field_instance} WHERE field_name = '%s'", $properties['field_name']);
  $type_names = array();
  while ($type = db_fetch_array($result)) {
    $type_names[] = $type['type_name'];
  }
  foreach ($type_names as $type_name) {
    content_field_instance_delete(array('type_name' => $type_name, 'field_name' => $properties['field_name']));
  }
  return (count($type_names) ? 1 : 0);
}


/**
 * Create a new field instance.
 *
 * @param $properties
 *   An array of properties to load the field instance with. Valid keys:
 *   - '' -
 * @return
 *   The ID of the newly-created field instance.
 */
function content_field_instance_create($properties) {
  // TODO
}

/**
 * Load a field instance.
 *
 * @param $properties
 *   An array of properties to use in selecting a field instance. Valid keys:
 *   - 'type_name' - The name of the content type in which the instance exists.
 *   - 'field_name' - The name of the field whose instance is to be loaded.
 * @return
 *   The field instance array.
 */
function content_field_instance_read($properties) {
  // TODO
}

/**
 * Update an existing field instance.
 *
 * @param $properties
 *   An array of properties to set in the field instance. Valid keys:
 *   - '' -
 * @return
 *   The number of field instance updated.
 */
function content_field_instance_update($properties) {
  // TODO
}

/**
 * Delete an existing field instance.
 *
 * @param $properties
 *   An array of properties to use in selecting a field instance. Valid keys:
 *   - 'type_name' - The name of the content type in which the instance exists.
 *   - 'field_name' - The name of the field whose instance is to be deleted.
 * @return
 *   The number of field instances deleted.
 */
function content_field_instance_delete($properties) {
  $number_deleted = db_query("DELETE FROM {node_field_instance} WHERE type_name = '%s' AND field_name = '%s'", $properties['type_name'], $properties['field_name']);

  $type = content_types($properties['type_name']);
  $field = $type['fields'][$properties['field_name']];
  $field_types = _content_field_types();
  $field_type = $field_types[$field['type']];
  $columns = module_invoke($field_type['module'], 'field_settings', 'database columns', $field);

  $instances = db_result(db_query("SELECT COUNT(*) FROM {node_field_instance} WHERE field_name = '%s'", $properties['field_name']));

  // If only one instance remains, we may need to change the database
  // representation for this field.
  if ($instances == 1) {
    if (!($field['multiple'])) {
      // Multiple-valued fields are always stored per-content-type.
      if (is_array($columns) && count($columns)) {
        $new_field = $field;
        $new_field['db_storage'] = CONTENT_DB_STORAGE_PER_CONTENT_TYPE;
        db_query("UPDATE {node_field} SET db_storage = %d WHERE field_name = '%s'", CONTENT_DB_STORAGE_PER_CONTENT_TYPE, $properties['field_name']);
        content_alter_db_field($field, $columns, $new_field, $columns);
      }
    }
  }

  // If no instances remain, delete the field entirely.
  elseif ($instances == 0) {
    if (is_array($columns) && count($columns)) {
      content_alter_db_field($field, $columns, array(), array());
    }
    db_query("DELETE FROM {node_field} WHERE field_name = '%s'", $properties['field_name']);
  }

  content_clear_type_cache();

  return $number_deleted;
}
