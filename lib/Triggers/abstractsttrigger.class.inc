<?php
abstract class PXAbstractStoragetypeTrigger extends PXAbstractTrigger {
	function __call($m, $a) {
		return true;
	}
}
?>
