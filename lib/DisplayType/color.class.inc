<?php
/**
 * Выбора цвета
 *
 * storageType string / VARCHAR(6)
 * displayType COLOR
 */
class PXDisplayTypeColor extends PXDisplayType {
	function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) ? htmlspecialchars($object[$field->name], ENT_COMPAT|ENT_HTML401, DEFAULT_CHARSET) : NULL;
		return NLAbstractHTMLForm::BuildColor($field->name, $value);
	}
	function buildCell($field, $object, $param=NULL) {
		$value = $object[$field->name];
		return '<div class="rgbcolor"><div style="background-color: #'.$value.';" title="#'.$value.'"></div></div>';
	}
}
?>