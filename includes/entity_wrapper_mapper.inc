<?php

/**
 * @file entity_wrapper_mapper.inc
 *
 * Provides a mapper for easy return of field data across entity references.
 */

/**
 * Extends the EntityMetadataWrapper
 *
 * Common usage is for services endpoints and node preprocessing
 * Summation of use: programmatically create and configure the object with the
 * base Entity and a map. Returns  referenced data in a custom array based on
 * the map.  The map consists key fields with values of nested arrays that
 * provide field level, fixed or property values.  These can be set to
 * drilldown thru connecting entity references to field level. Multiple value
 * lists are itereated automatically although the first value can be specified
 * as the default.
 */
class EntityWrapperMapper {

  var $info;
  var $entity_wrapper;
  var $custom_vars;
  var $errmsg;
  var $map;
  private $field_wrapper;
  private $type;

  public function create() {
    $instance = new static();
    return ($instance);
  }

  public function setInfo(array $info) {
    $this->info = $info;
  }

  public function setWrapper($wrapper) {
    $this->entity_wrapper = $wrapper;
  }

  public function setCustomVars($custom_vars) {
    $this->custom_vars = $custom_vars;
  }

  public function setErrMsg($errmsg = TRUE) {
    $this->errmsg = $errmsg;
  }

  public function setMap($map) {
      $this->map = $map;
  }

/**
 * Starts the mapping process.
 * 
 * @return array
 */  
  public function execute() {
    $data = $this->mapData($this->entity_wrapper,$this->map);
    return $data;
  }

/**
 * Iterates through a map and returns a value.
 * 
 * @param type $ew
 * @param type $map
 * @return array
 */ 
  public function mapData($ew, $map) {
    foreach($map as $element => $info) {
      $ref_field[$element] = $this->fieldHandler($ew, $info);
    }
    return $ref_field;
  }

/**
 * Routes based on the field type as list or single value.
 * 
 * @param type $ew
 * @param type $info
 * @return array
 */
  private function fieldHandler($ew, $info) {
    if (isset($info['field_name'])) {
      $field_name = $info['field_name'];
      //handle this better as extension
      if (isset($ew->$field_name)) {
        $fw = $ew->$field_name;
        $fw_info = $fw->info();
        $left_list = substr($fw_info['type'],0,5);
        if ($left_list == 'list<') {
          $ref_field = $this->listHandler($ew, $info);
        }
        else {
          $ref_field = $this->valueByField($ew, $info);
        }
      }
      else {
        $ref_field = $this->valueByField($ew, $info);
      }
    }
    else {
      $ref_field = $this->valueByField($ew, $info);
    }
    return $ref_field;
  }

/**
 * For list fields (e.g. cardinality > 1, multi-value) this handles the routing 
 * for value vs drilldown vs new mapping.
 * 
 * @param type $ew
 * @param type $info
 * @return array
 */  
  private function listHandler($ew, $info) {
    $field_name = $info['field_name'];
    $ew = $ew->$field_name;
    // there are 3 possiblities.  Either the type is a drilldown or it is not.
    // if it is a drilldown than the mapping can either be another
    // full map or a single type.  Here is whence we determine such.
    $map_keys = isset($info['mapping']) ? array_keys($info['mapping']) : array();
    if ($info['type'] == 'drilldown' && $map_keys[0] == 'type') {
      $list_info = $info['mapping'];
      foreach ($ew->getIterator() as $delta => $item) {
        $values[$delta] = $this->fieldHandler($item, $list_info);
      }
    }
    elseif ($info['type'] == 'drilldown') {
      $list_info = $info['mapping'];
      foreach ($ew->getIterator() as $delta => $item) {
        $values[$delta] = $this->mapData($item, $list_info);
      }
    }
    else {
      foreach ($ew->getIterator() as $delta => $item) {
        $values[$delta] = $this->valueByField($item, $info);
      }
    }
    // handles when we know this is a list
    // and want only a default value of the first item
    if (isset($info['list_item']) && $info['list_item'] == 'first') {
        $values = $values[0];
    }

    return $values;
  }
  
/**
 * Routes based on the 'type' field or if there is a callback.
 * 
 * @param type $ew
 * @param type $info
 * @return type * 
 */
  public function valueByField($ew, $info) {
    if (isset($info['args']['callback'])) {
      $callback = $info['args']['callback'];
      $callback($ew, $info, $this->custom_vars, $this->entity_wrapper);
    }
    if (isset($info['field_name'])) {
      $field_name = $info['field_name'];
      try {
        if ($this->checkFieldExists($ew, $info)) { return $value = ''; }
      }
      catch (Exception $e) {
        watchdog('php', $e);
      }
    }
    switch ($info['type']) {
      case 'render':
        $value = $this->valueRender($ew, $info);
        break;

      case 'blank':
        $value = '';
        break;

      case 'custom':
        $value = $this->valueCustom($ew, $info);
        break;

      case 'value':
        $value = $this->valueByType($ew, $info);
        break;

      case 'image_url':
        if (isset($ew->$field_name)) {
          $file_array = $ew->$field_name->value();
          if ($info['args']['style'] != 'original') {
            $style = strtolower($info['args']['style']);
            $image_url_wrapper = image_style_url($style, $file_array->uri);
          }
          else {
            $image_url_wrapper = $file_array->uri;
          }
          $image_url = $this->_ewm_file_create_url($image_url_wrapper);
          $value = $image_url;
        }
        else {
          $value = '';
        }
        break;

      case 'alias_url':
        $id = !empty($field_name) ? $ew->$field_name->getIdentifier() : $ew->getIdentifier();
        $alias = drupal_get_path_alias('node/'.$id);
        $path = $alias ? $alias :'node/'.$id;
        $domain = $this->_ewm_current_domain();
        $value = $domain . '/' . $path;
        break;


      case 'fixed';
        $value = $info['value'];
        break;

      case 'concatenate';
        foreach ($info['fields'] as $fields_info) {
          $values[] = $this->valueByField($ew, $fields_info);
        }
        $value = implode($values);
        break;
//      case 'token-replacement';
//        $value =  $this->get_token_replacement($entity_wrapper, $info);
//        break;

      case 'default':
        $value = '';
        break;
    }
    if ($info['type'] == 'drilldown') {
      $field_name = $info['field_name'];
      $ew = $ew->$field_name;
      $info = $info['mapping'];
      $value = $this->fieldHandler($ew, $info);
    }
    if (empty($value) && !empty($info['alt_fields'])) {
       $value = $this->checkAlt($ew, $info);
    }

    if (!isset($value)) {
      $x = 1;
    }
    $value = is_null($value) ? '' : $value;

    return $value;
  }

  private function valueRender($ew, $info) {
    $field = $ew->$info['field_name'];
    $type = $field->info('type');
    $field_value = $field->value();

    $value = field_view_value($type, $ew, $info['field_name'], $field_value);

    return $value;
  }

  public function checkFieldExists($ew, $info) {
    $field_name = $info['field_name'];
    // Entity_metadata_wrapper has one giant flaw. Trying to ref a field that
    // isn't set causes an error.  We avoid this here.
    if (is_a($ew, 'EntityDrupalWrapper') && !empty($field_name)) {
      // maybe just a try and throw here is good or extend the emw
      if (!isset($ew->$field_name) && $this->errmsg == TRUE) {
        $this->post_error($ew, $info, 'unset_field');
        return TRUE;
      }
    }
    else {
      return FALSE;
    }
  }

  private function checkAlt($ew, $info) {
    $info['field_name'] = array_shift($info['alt_fields']);
    $value = $this->valueByField($ew, $info);
    return $value;
  }

/**
 * Retrieves the value of the field based on type.
 * 
 * The type is derived from the Entity Metadata Wrapper info() function.
 * 
 * @param type $ew
 * @param type $info
 * @return type 
 */  
  public function valueByType($ew, $info) {
    $field_name = $info['field_name'];
    // check the wrapper class, its possible to for $ew to be a
    // EntityDrupalWrapper or an EntityValueWrapper
    if (get_class($ew) == 'EntityValueWrapper') {
      $field = $ew;
    }
    else {
      $field = $ew->$field_name;
    }
    $props = $field->info();
    $type = $props['type'];

    switch ($type) {
      case 'text_formatted':
        try{
          if ($field->__isset('value')) {
            $field_format = $field->format->value();
            $default_value = $field->value->value();
            $field_raw = $field->value->raw();
            if (!empty($info['args']['format']) && $info['args']['format'] != 'raw') {
              $field_format = $info['args']['format'];
              $field_value = check_markup($field_raw, $field_format);
            }
            elseif (!empty($info['args']['format']) && $info['args']['format'] == 'raw') {
              $field_value = $field_raw;
            }
            else {
              $decoded_value = htmlspecialchars_decode($default_value);
              $stripped_value = strip_tags($decoded_value);
              $field_value = $stripped_value;
            }
            $value = str_replace(PHP_EOL, '', $field_value);
          }
          else {
            $value = '';
          }
        }
        catch (Exception $e) {
          watchdog('php', $e);
          $value = '';
        }
        break;

      case 'text':
        $value = strip_tags(str_replace(PHP_EOL, '', $field->value()));
        break;

      case 'integer':
        $value = $field->value();
        break;

      case 'taxonomy_term':
        $id = $field->value();
        if (isset($id)) {
          $value = $field->value()->name;
        }
        else {
          $value = '';
        }
        break;

      case 'date':
        $format = $info['format'];
        $value = $field->value()?format_date($field->value(), $format):'';
        break;

      default:
        $value = $type;
    }
    $value = empty($value) ? '' : $value;
    return $value;
  }

/**
 * Returns the value based on the custom_type.
 * 
 * @param EntityMetadataWrapper object $ew
 * @param array $info
 * @return string $value
 */
  private function valueCustom($ew, $info) {
    // Allows for the use of a callback
    if (isset($info['args']['callback'])) {
      $callback = $info['args']['callback'];
      $callback($ew, $info, $this->custom_vars, $this->entity_wrapper);
    }
    
    switch ($info['args']['custom_type']) {
      case 'callback_value' :
        $value = $this->valueByField($ew, $info);
        break;

      case 'list':
        $all_fields = field_info_fields();
        $list = list_allowed_values($all_fields[$info['field_name']]);
        $value = $list[$ew->$info['field_name']->value()];
        break;

      case 'image_data_type':
        $value = (string) $this->get_image_dimensions($ew, $info);
        break;

      case 'index':
        $value = (string) $this->custom_vars['delta'];
        break;

      case 'boolean_if_isset':
        $field_value = $ew->$info['field_name']->value();
        $value = !empty($field_value) ? TRUE : FALSE;
        break;

      case 'customvar_value' :
        $value = $this->custom_vars[$info['field_name']];

      case 'custom_value':
        $value = $this->custom_vars[$info['value']];
        break;
    }
    $value = is_null($value) ? '' : $value;
    return $value;
  }

/**
 * Returns the width or height of an image
 * 
 * @param object $entity_wrapper
 * @param array $info
 * @return array $image_data
 */  
  private function get_image_dimensions($entity_wrapper, $info) {
    if (isset($entity_wrapper->$info['field_name'])) {
      $file_array = $entity_wrapper->value();
      $style = strtolower($info['args']['style']);
      $image_url_wrapper = image_style_path($style, $file_array['uri']);
      $image_data = image_get_info($image_url_wrapper);
      switch ($info['args']['custom_data_type']) {
        case 'width':
          return $image_data['width'];

        case 'height':
          return $image_data['height'];
       }
    }
    else {
      $value = '';
      return $value;
    }
  }

/**
 * Returns the url for a file.
 * 
 * @param type $uri
 * @return type
 */
  private function _ewm_file_create_url($uri) {
    ///replace with cdn module reference
    $env_domain = $this->_ewm_current_domain() . '/sites/default/files/';
    $drupal_url = str_replace('public://', $env_domain, $uri);

    return $drupal_url;
  }

/**
 * Returns the Current Domain.
 * 
 * @return string
 */
  private function _ewm_current_domain() {
    $domain = $_SERVER['REQUEST_SCHEME'] . '://'.$_SERVER['SERVER_NAME'];
    return $domain;
  }

/**
 * Creates error text to be posted to the watchdog log.
 * 
 * @param type $ew
 * @param type $info
 * @param type $err_type
 * @param type $e
 */
  private function post_error($ew, $info, $err_type = null , $e = null) {
    switch ($err_type) {
      case 'unset_field':
        $entity_bundle = isset($ew->bundle) ? $ew->getBundle : 'no bundle';
        $error_text = 'EntityWrapperMapper tried to map to a field '
        . 'that is not assocaited with the referenced Entity:'
        . $entity_bundle . "   Field Name:" . $info['field_name'];
        break;

      case $default:
        $error_text = $e;
        break;
    }
    // Suppress error message
    //drupal_set_message($error_text, 'error');
  }

}


