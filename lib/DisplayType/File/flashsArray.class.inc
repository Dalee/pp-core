<?php
/**
 * Список флэшей
 *
 * storageType flashsarray
 * displayType FLASHSARRAY
 */
class PXDisplayTypeFlashsarray extends PXDisplayTypeImagesarray {
	function buildArrayItem($name, $value) {
		return NLAbstractHTMLForm::BuildMultimedia($name, $value, 'flash', $value['entry']);
	}

	function buildCellHtml($value, $id) {
		return PXDisplayTypeFlash::buildCellHtml($value, $id);
	}
}
?>