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

	function buildCellHtml($value, $id, $dummy = null) { /* E_STRICT warning remove */
		return PXDisplayTypeFlash::buildCellHtml($value, $id);
	}
}
?>