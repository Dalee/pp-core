<?php

abstract class PXAbstractSearchTrigger extends PXAbstractTrigger {
	function onBeforeSearchStems($config, &$where) {
	}

	function onBeforeSearchObjects($formatid, &$where) {
	}

	function onAfterSearchObjects($formatid, &$row) {
	}

	function onIndexCronGetSelectCols($formatid, &$cols) {
	}

	function onIndexCronStemWeight($formatid, &$stemW, &$row) {
	}

	function onIndexCronCanIndexRow($formatid, &$row, &$tree) {
		return true;
	}
}

