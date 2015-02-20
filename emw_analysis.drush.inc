<?php
/**
 * @file
 * Drush command to test the speed of various field data retrieval methods.
 */

/**
 * Implements hook_drush_command().
 */
function emw_analysis_drush_command() {
  $items['emw_analysis'] = array(
    'description' => 'Test the performance of various field data retrieval methods.',
    'aliases' => array('emwa'),
  );
  return $items;
}

/**
 * Checks the speeds of various field data retrieval methods.
 *
 * Tests performance of loading field values using three different methods.
 *   - $node->field_shared_short_title[LANGUAGE_NONE][0]['value'];
 *   - entity_metadata_wrapper('node', $node);
 *   - field_get_items('node', $node, 'field_shared_short_title');
 *
 * Requires nodes of type 'page' with a field named 'field_test_value' on the
 * given node type to allow the test to run.
 */
function drush_emw_analysis() {
  // Define the field name to use.
  $field_name = 'field_test_value';
  // Define the node type to test.
  $node_type = 'page';
  // Load all of the nodes.
  $nodes = node_load_multiple(array(), array('type' => $node_type));
  // Record the inital memory usage.
  $memory_0 = memory_get_peak_usage();

  // Use the standard array.
  $start = microtime(TRUE);
  foreach ($nodes as $key => $node) {
    if (!empty($node->{$field_name}[LANGUAGE_NONE][0]['value'])) {
      $value = $node->{$field_name}[LANGUAGE_NONE][0]['value'];
    }
  }
  $end = microtime(TRUE);
  $memory_1 = memory_get_peak_usage();
  $time = $end - $start;
  $time1 = $time;
  drush_log('standard array');
  drush_log('start: ' . $start);
  drush_log('end: ' . $end);
  drush_log('time: ' . $time);

  // Use entity_metadata_wrapper.
  $start = microtime(TRUE);
  foreach ($nodes as $key => $node) {
    $wrapped_node = entity_metadata_wrapper('node', $node);
    $value = $wrapped_node->{$field_name}->value();
    $wrapped_node->notadf()->value();
  }
  $end = microtime(TRUE);
  $memory_2 = memory_get_peak_usage();
  $time = $end - $start;
  $time2 = $time;
  drush_log('entity_metadata_wrapper');
  drush_log('start: ' . $start);
  drush_log('end: ' . $end);
  drush_log('time: ' . $time);

  // Use field_get_items.
  $start = microtime(TRUE);
  foreach ($nodes as $key => $node) {
    $value = field_get_items('node', $node, $field_name);
  }
  $end = microtime(TRUE);
  $memory_3 = memory_get_peak_usage();
  $time = $end - $start;
  $time3 = $time;
  drush_log('field_get_items');
  drush_log('start: ' . $start);
  drush_log('end: ' . $end);
  drush_log('time: ' . $time);

  // Use field_view_value.
  $start = microtime(TRUE);
  foreach ($nodes as $key => $node) {
    $value = field_view_value('node', $node, $field_name, 0);
  }
  $end = microtime(TRUE);
  $memory_4 = memory_get_peak_usage();
  $time = $end - $start;
  $time4 = $time;
  drush_log('field_get_items');
  drush_log('start: ' . $start);
  drush_log('end: ' . $end);
  drush_log('time: ' . $time);

  drush_log('--------------------------------', 'success');
  drush_log('entity_metadata_wrapper vs. array', 'success');
  drush_log(round($time2/$time1, 3) . ' times longer', 'success');
  drush_log(round($time2-$time1, 3) . ' seconds longer', 'success');
  drush_log('array memory usage: ' . format_bytes($memory_1 - $memory_0), 'success');

  drush_log('--------------------------------', 'success');
  drush_log('field_get_items vs. array', 'success');
  drush_log(round($time3/$time1, 3) . ' times longer', 'success');
  drush_log(round($time3-$time1, 3) . ' seconds longer', 'success');
  drush_log('entity_metadata_wrapper memory usage: ' . format_bytes($memory_2 - $memory_1), 'success');

  drush_log('--------------------------------', 'success');
  drush_log('entity_metadata_wrapper vs. field_get_items', 'success');
  drush_log(round($time2/$time3, 3) . ' times longer', 'success');
  drush_log(round($time2-$time3, 3) . ' seconds longer', 'success');
  drush_log('field_get_items memory usage: ' . format_bytes($memory_3 - $memory_2), 'success');

  drush_log('--------------------------------', 'success');
  drush_log('field_view_value vs. field_get_items', 'success');
  drush_log(round($time4/$time3, 3) . ' times longer', 'success');
  drush_log(round($time4-$time3, 3) . ' seconds longer', 'success');
  drush_log('field_get_items memory usage: ' . format_bytes($memory_4 - $memory_3), 'success');
}

/**
 * Format the bytes data into a human readable format.
 *
 * Taken from stackoverlow. @see http://stackoverflow.com/questions/2510434.
 */
function format_bytes($bytes, $precision = 2) {
    $units = array('B', 'KB', 'MB', 'GB', 'TB');
    $bytes = max($bytes, 0);
    $pow = floor(($bytes ? log($bytes) : 0) / log(1024));
    $pow = min($pow, count($units) - 1);
    // Uncomment one of the following alternatives
    $bytes /= pow(1024, $pow);
    // $bytes /= (1 << (10 * $pow));
    return round($bytes, $precision) . ' ' . $units[$pow];
}
