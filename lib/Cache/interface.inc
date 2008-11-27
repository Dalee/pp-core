<?php
interface CacheIF {
	function exists($objectId);
	function save($objectId, $data);
	function load($objectId);
	function clear();
}
?>