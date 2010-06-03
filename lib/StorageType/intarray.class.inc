<?php
class PXStorageTypeIntArray extends PXStorageTypeArray {
	function valueToDB($k, $v) {
		return ($v === '' || is_null($v)) ? 'null' : (int)$v;
	}

}

?>
