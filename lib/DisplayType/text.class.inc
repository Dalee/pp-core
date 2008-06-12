<?php
/**
 * Текстовое поле
 *
 * storageType string, integer или float
 * displayType TEXT|width|height
 *             width не используется
 *             если height = 0, то строится <input type="text">
 *             если height > 0, то строится <textarea> и height это ее высота в пикселях
 */
class PXDisplayTypeText extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) ? htmlspecialchars($object[$field->name]) : NULL;
		return NLAbstractHTMLForm::BuildText($field->name, $value, $field->displayTypeArgs[1]);
	}

	static function buildCell($field, $object, $param=NULL) {
		$value = strip_tags($object[$field->name]);
		return strlen($value) > 50 ? substr($value, 0, 50).'&hellip;' : $value;
	}
}
?>
