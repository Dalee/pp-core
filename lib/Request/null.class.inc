<?php
class PXRequestNull extends PXRequestAbstract {
	function __construct() {
	}

	function getVar($varName) {
		return NULL;
	}
}
?>
