<?php

/**
 * Выбора цвета
 *
 * storageType string / VARCHAR(6)
 * displayType COLOR
 */
class PXDisplayTypeColor extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) ? htmlspecialchars($object[$field->name]) : NULL;
		return NLAbstractHTMLForm::BuildColor($field->name, $value);
	}
	static function buildCell($field, $object, $param=NULL) {
		$value = $object[$field->name];
		return '<div class="rgbcolor"><div style="background-color: #'.$value.';" title="#'.$value.'"></div></div>';
	}
}
?>
