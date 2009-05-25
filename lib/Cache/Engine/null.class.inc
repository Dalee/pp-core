<?php
class PXCacheNull implements IPXCache {
	function exists($objectKey){}
	function save($objectKey, $dataForSave, $expirationTime = null){}
	function load($objectKey){}
	function delete($objectKey){}
	function clear(){}
}
?>