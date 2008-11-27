<?php
class NullCache implements CacheIF {
	function NullCache() {}
	function exists($objectId){}
	function save($objectId, $data){}
	function load($objectId){}
	function clear(){}
}
?>
