<?php

abstract class PXAbstractLayoutTrigger extends PXAbstractTrigger {
	function OnBeforePathCreated($tree, $id) {
	}

	function OnAfterPathCreated($createdPath) {
		return $createdPath;
	}
}
