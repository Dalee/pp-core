<?php
class PXLayoutInterface {
	function __construct() {
	}

	function assign($variable, $value) {
	}

	function setApp($app) {
	}

	function setLang($lang='rus') {
	}

	function setContent () {
	}

	function getContent () {
	}
}

class PXLayoutNull extends PXLayoutInterface {
}
?>
