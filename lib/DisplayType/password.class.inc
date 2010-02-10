<?php
/**
 * Поле для смены пароля
 *
 * storageType string
 * displayType PASSWORD
 */
class PXDisplayTypePassword extends PXDisplayType {
	function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) && strlen($object[$field->name]) ? $object[$field->name] : NULL;
		return NLAbstractHTMLForm::BuildPassword($field->name, $value);
	}

	function buildCell($field, $object, $param=NULL) {
		return strlen($object[$field->name]) ? PXDisplayTypeCheckbox::buildCell($field, array($field->name => true), $param) : '';
	}

	function getFromRequest($field, $object, $param=NULL) {
		$tmp = $param['request']->postData->_GetArrayVar($field->name);

		$value = (isset($tmp['type']) && isset($tmp['retype']) && strlen($tmp['type']) && ($tmp['type'] === $tmp['retype'])) ? trim($tmp['type']) : null;
		if(strlen($value) && !is_null($value)) {
			$authmethodname = PXRegistry::getUser()->getPrimaryAuthMethod($field->displayTypeArgs[0]);
			$value = call_user_func(array($authmethodname, 'passwdToDB'), $value);
		}

		if(isset($tmp['delete']) && $tmp['delete'] === 'on') {
			$value = "\n\t\n";
		}

		return $value;
	}

	function preModifyObject($field, &$object, $param=NULL) {
		if(!strlen($object[$field->name])) {
			return true;
		}

		if($object[$field->name] === "\n\t\n") {
			$object[$field->name] = null;
		}
	}
}
?>