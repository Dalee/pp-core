<?php
/**
 * Checkbox
 *
 * storageType boolean
 * displayType CHECKBOX
 */
class PXDisplayTypeCheckbox extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$value = (isset($object[$field->name])  && $object[$field->name] == true) ? true : false;
		return NLAbstractHTMLForm::BuildInputCheckBox($field->name, $value);
	}

	static function buildCell($field, $object, $param=NULL) {
		$value = $object[$field->name];
		return $value ? '<div class="truecheck" title="да"></div>' : '';
	}

	static function getFromRequest($field, $object, $param=NULL) {
		return $param['request']->postData->_GetCheckBoxVar($field->name);
	}
}
?>
