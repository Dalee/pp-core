<?php
abstract class PXAbstractSearchTrigger extends PXAbstractTrigger {
	function onBeforeSearchStems($config, array &$where) {
	}

	function onBeforeSearchObjects($formatid, array &$where) {
	}

	function onIndexCronGetSelectCols($formatid, array &$cols) {
	}

	function onIndexCronStemWeight($formatid, array &$stemW, array $row) {
	}
}
?>
