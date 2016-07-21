--TEST--
Check geo_encode / geo_decode
--SKIPIF--
<?php if (!extension_loaded("geo")) print "skip"; ?>
--FILE--
<?php 
$lat = 40;
$lng = 139;
$r1 = geo_encode($lat, $lng, 7);
$r2 = geo_encode($lat, $lng, 6);
$r3 = geo_encode($lat, $lng, 5);
$r4 = geo_encode($lat, $lng, 4);
$r5 = geo_encode($lat, $lng, 3);

$d1 = geo_decode($r1);
$d2 = geo_decode($r2);
$d3 = geo_decode($r3);
$d4 = geo_decode($r4);
$d5 = geo_decode($r5);


var_export($r1); echo "\n";
var_export($r2); echo "\n";
var_export($r3); echo "\n";
var_export($r4); echo "\n";
var_export($r5); echo "\n";
var_export($d1); echo "\n";
var_export($d2); echo "\n";
var_export($d3); echo "\n";
var_export($d4); echo "\n";
var_export($d5); echo "\n";
?>
--EXPECT--
'xp4ge1b'
'xp4ge1'
'xp4ge'
'xp4g'
'xp4'
array (
  'lat' => 40.000534057617188,
  'lng' => 138.99971008300781,
  'north' => 40.001220703125,
  'south' => 39.999847412109375,
  'east' => 139.00039672851562,
  'west' => 138.9990234375,
)
array (
  'lat' => 39.99847412109375,
  'lng' => 139.0045166015625,
  'north' => 40.001220703125,
  'south' => 39.9957275390625,
  'east' => 139.010009765625,
  'west' => 138.9990234375,
)
array (
  'lat' => 40.01220703125,
  'lng' => 139.02099609375,
  'north' => 40.0341796875,
  'south' => 39.990234375,
  'east' => 139.04296875,
  'west' => 138.9990234375,
)
array (
  'lat' => 39.990234375,
  'lng' => 139.04296875,
  'north' => 40.078125,
  'south' => 39.90234375,
  'east' => 139.21875,
  'west' => 138.8671875,
)
array (
  'lat' => 40.078125,
  'lng' => 138.515625,
  'north' => 40.78125,
  'south' => 39.375,
  'east' => 139.21875,
  'west' => 137.8125,
)
