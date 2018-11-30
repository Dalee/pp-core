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

	function buildCellHtml() {
		list($value, $id) = func_get_args(); // PHP54 args contract violation
		return (new PXDisplayTypeFlash($this->display, $this->name))->buildCellHtml($value, $id);
	}
}
?>