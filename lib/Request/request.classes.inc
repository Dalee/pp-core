<?php
class PXNullRequest extends PXRequestAbstract {
	function __construct() {
	}

	function getVar($varName) {
		return NULL;
	}
}

class PXRequestAbstract extends PXRequest {}
class PXRequestBase     extends PXRequestAbstract {}
class PXRequestUser     extends PXRequestBase {}
?>