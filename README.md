EntityWrapperMapper
===================

An extension of the EntityMetadataWrapper that returns a custom data array 
based on a map (configuration array).  This module was created in order to more
easily access fields that are referenced across multiple entities.

Use
===
Common usage is for services endpoints and node preprocessing

How to Implement (Summary)
==========================
Programmatically create and configure an EntityWrapperMapper object with the
base Entity and a map. Returns referenced data in a custom array based on
the map.  The map consists of key fields with values of nested arrays that
provide field level, fixed, property or custom values.  These can be set to
drilldown thru connecting entity references to the field level. Multiple value
lists are iterated automatically and the first value can be specified
 as the default.

The EWM Object configuration
============================
The only requirement for the configuration of the EWM objects are an
EntityMetadataWrapper and a map.   These are configured using the the setMap()
and setWrapper() methods.  The execute() method is used to retrieve the values.

Example: EWM Object Configuration
=================================

  $nid = $node->nid;
  $node_wrapper = entity_metadata_wrapper('node', $nid);

  $ewm = new EntityWrapperMapper();
  $ewm->setMap($main_map);
  $ewm->setWrapper($node_wrapper);
  $node_fields = $ewm->execute();

The EWM Map
===========
The map is an associative array.  The EWM returns an associative array that is
derived from the map.  The keys of both the original map and the returned array
are the same.

The values of the returned array are derived from the map.  The most basic map
would contain a key (name) and an array of a type and field_name:

    $map = array(
        'example_name' => array(
            'type' => 'value',
            'field_name' => 'field_example',
        ),
    );

Given the field_example field is a simple text field with a value of 
'exampleValue' this map would return an array that would look like this:
array(example_name => 'exampleValue')

The 'type'
================
There are five allowed values for the 'type'.  They are 'blank', 'value', 
'drilldown', 'alias_url', 'image_url', 'concatenate' and 'custom'.  

The 'value' will return the value of the field.  This will be whatever is most
commonly expected for the type of field so a text field will return the text
value and a taxonomy field will return the taxonomy term.  It is customizable 
for certain types of fields such as a long text field by using the args key 
(see the following section for more about args).

Example
=======
The following example is taken from a preprocessor. First is the map:
//////////////////////////
  $main_node_map = array(
    'main_header' => array(
      'type' => 'value',
      'field_name' => 'field_header',
    ),
    'main_image_url' => array(
      'type' => 'drilldown',
      'field_name' => 'field_images',
      'list_item' => 'first',
      'mapping' => array(
        'type' => 'drilldown',
        'field_name' => 'field_image',
        'mapping' => array(
          'type' => 'image_url',
          'field_name' => 'file',
          'args' => array(
            'style' => 'original',
          ),
        ),
      ),
    ),
    'main_image_caption' => array(
      'type' => 'value',
      'field_name' => 'field_image_caption',


  $node = $variables['node'];
  $nid = $node->nid;
  $node_wrapper = entity_metadata_wrapper('node', $nid);

  $ewm = new EntityWrapperMapper();
  $ewm->setMap($main_map);
  $ewm->setWrapper($node_wrapper);
  $node_fields = $ewm->execute();

  return $node_fields;

Drilldown & 'mapping'
=====================

Using 'args'
============
For some field types there is a requirement to provide additional information.
This is provided in a separate array that uses the 'args' key.

For each 'type' the 'args', if they are required, are generally different. 
