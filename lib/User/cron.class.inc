<?php

class PXUserCron extends PXUser {
	function aclType() {
		return 'GOD';
	}

	function can($what, $format = NULL, $object = NULL) {
		return true;
	}
}
