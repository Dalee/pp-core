<?php
/**
 * ���� ��� ����� ������
 *
 * storageType string
 * displayType PASSWORD
 */
class PXDisplayTypePassword extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) && strlen($object[$field->name]) == 32 ? $object[$field->name] : NULL;
		return NLAbstractHTMLForm::BuildPassword($field->name, $value);
	}

	static function buildCell($field, $object, $param=NULL) {
		return strlen($object[$field->name]) == 32 ? PXDisplayTypeCheckbox::buildCell($field, array($field->name => true), $param) : '';
	}

	static function getFromRequest($field, $object, $param=NULL) {
		$tmp = $param['request']->postData->_GetArrayVar($field->name);

		$value = (isset($tmp['type']) && isset($tmp['retype']) && strlen($tmp['type']) && ($tmp['type'] === $tmp['retype'])) ? trim($tmp['type']) : null;

		if(strlen($value) && !is_null($value)) {
			$value = md5($value);
		}

		if(isset($tmp['delete']) && $tmp['delete'] === 'on') {
			$value = "\n\t\n";
		}

		return $value;
	}

	static function preModifyObject($field, &$object, $param=NULL) {
		if(!strlen($object[$field->name])) {
			return true;
		}

		if($object[$field->name] === "\n\t\n") {
			$object[$field->name] = null;
		}
	}
}
?>