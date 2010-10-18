<?php

abstract class PXAbstractLayoutTrigger extends PXAbstractTrigger {

	function OnAfterPathCreated($createdPath) {
		return $createdPath;
	}

}

?>