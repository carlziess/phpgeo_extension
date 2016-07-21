--TEST--
Check geo_simple_distance/geo_distance
--SKIPIF--
<?php if (!extension_loaded("geo")) print "skip"; ?>
--FILE--
<?php 

var_export(geo_simple_distance(30.635957,104.011712,30.576722,104.071445)); echo "\n";
var_export(geo_distance(30.635957,104.011712,30.576722,104.071445)); echo "\n";

?>
--EXPECT--
6839.2073302854624
8731.2457741010312
