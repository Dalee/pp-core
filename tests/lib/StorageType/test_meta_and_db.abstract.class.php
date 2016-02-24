<?php
set_time_limit(0);
require_once __DIR__.'/../../../lib/maincommon.inc';

ini_set('display_errors', '1');

// old
class testOldNoMetaAndNoDb extends PXStorageType {
	function notInDb($field, $object, $param = NULL) {
		return true;
	}
	function inMeta($field, $object, $param = NULL) {
		return false;
	}
}
class testOldNoMetaAndDb extends PXStorageType {
	function notInDb($field, $object, $param = NULL) {
		return false;
	}
	function inMeta($field, $object, $param = NULL) {
		return false;
	}
}
class testOldMetaAndNoDb extends PXStorageType {
	function notInDb($field, $object, $param = NULL) {
		return true;
	}
	function inMeta($field, $object, $param = NULL) {
		return true;
	}
}
class testOldMetaAndDb extends PXStorageType {
	function notInDb($field, $object, $param = NULL) {
		return false;
	}
	function inMeta($field, $object, $param = NULL) {
		return true;
	}
}
class testOldNoMeta extends PXStorageType {
	function inMeta($field, $object, $param = NULL) {
		return false;
	}
}
class testOldMeta extends PXStorageType {
	function inMeta($field, $object, $param = NULL) {
		return true;
	}
}
class testOldNoDb extends PXStorageType {
	function notInDb($field, $object, $param = NULL) {
		return true;
	}
}
class testOldDb extends PXStorageType {
	function notInDb($field, $object, $param = NULL) {
		return false;
	}
}
class testOldEmpty extends PXStorageType {
}
// new
class testNewNoMetaAndNoDb extends PXStorageType {
	function storedInDb() {
		return false;
	}
	function storedInMeta() {
		return false;
	}
}
class testNewNoMetaAndDb extends PXStorageType {
	function storedInDb() {
		return true;
	}
	function storedInMeta() {
		return false;
	}
}
class testNewMetaAndNoDb extends PXStorageType {
	function storedInDb() {
		return false;
	}
	function storedInMeta() {
		return true;
	}
}
class testNewMetaAndDb extends PXStorageType {
	function storedInDb() {
		return true;
	}
	function storedInMeta() {
		return true;
	}
}
class testNewNoMeta extends PXStorageType {
	function storedInMeta() {
		return false;
	}
}
class testNewMeta extends PXStorageType {
	function storedInMeta() {
		return true;
	}
}
class testNewNoDb extends PXStorageType {
	function storedInDb() {
		return false;
	}
}
class testNewDb extends PXStorageType {
	function storedInDb() {
		return true;
	}
}
class testNewEmpty extends PXStorageType {
}


$field = new PXFieldDescription(null, null, null);
$classes = [];

//old
$classes[] = new testOldNoMetaAndNoDb($field, 'testNoMetaAndNoDb');
$classes[] = new testOldNoMetaAndDb($field, 'testNoMetaAndDb');
$classes[] = new testOldMetaAndNoDb($field, 'testMetaAndNoDb');
$classes[] = new testOldMetaAndDb($field, 'testMetaAndDb');
$classes[] = new testOldNoMeta($field, 'testNoMetaAndDb');
$classes[] = new testOldMeta($field, 'testMetaAndDb');
$classes[] = new testOldNoDb($field, 'testMetaAndNoDb');
$classes[] = new testOldDb($field, 'testNoMetaAndDb');
$classes[] = new testOldEmpty($field, 'testNoMetaAndDb');
//new
$classes[] = new testNewNoMetaAndNoDb($field, 'testNoMetaAndNoDb');
$classes[] = new testNewNoMetaAndDb($field, 'testNoMetaAndDb');
$classes[] = new testNewMetaAndNoDb($field, 'testMetaAndNoDb');
$classes[] = new testNewMetaAndDb($field, 'testMetaAndDb');
$classes[] = new testNewNoMeta($field, 'testNoMetaAndDb');
$classes[] = new testNewMeta($field, 'testMetaAndDb');
$classes[] = new testNewNoDb($field, 'testMetaAndNoDb');
$classes[] = new testNewDb($field, 'testNoMetaAndDb');
$classes[] = new testNewEmpty($field, 'testNoMetaAndDb');

print('Call old methods:'.PHP_EOL);
foreach ($classes as $class) {
	$test = sprintf('test%sMetaAnd%sDb', $class->inMeta(null, null) ? '' : 'No', !$class->notInDb(null, null) ? '' : 'No');
	$should = $class->name;
	print(sprintf('  [%s] - <%s> get "%s" should "%s"'.PHP_EOL, $test === $should ? 'OK' : 'ERROR', get_class($class), $test, $should));
}
print('Call new methods:'.PHP_EOL);
foreach ($classes as $class) {
	$test = sprintf('test%sMetaAnd%sDb', $class->storedInMeta() ? '' : 'No', $class->storedInDb() ? '' : 'No');
	$should = $class->name;
	print(sprintf('  [%s] - <%s> get "%s" should "%s"'.PHP_EOL, $test === $should ? 'OK' : 'ERROR', get_class($class), $test, $should));
}
