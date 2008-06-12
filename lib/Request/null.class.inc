<?php
class PXRequestNull extends PXRequestAbstract {
	function __construct() {
	}

	function getVar() {
		return NULL;
	}
}
?>