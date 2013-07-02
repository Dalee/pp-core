<?php

interface IPXCache {
	function exists($objectKey);
	function save($objectKey, $dataForSave, $expirationTime = null);
	function load($objectKey);
	function delete($objectKey);
	function clear();
	function increment($numberKey, $offset = 1 , $initial = 0, $expirationTime = null);
}

?>