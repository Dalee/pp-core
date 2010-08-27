<?php
class PXDisplayTypeMultiRegions_Static extends PXDisplayTypeMultiRegions {
	function buildInput($field, $object, $param=NULL) {
		return $this->buildCell($field, $object, $param);
	}
}
?>
