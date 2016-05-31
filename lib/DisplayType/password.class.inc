<?php
/**
 * Поле для смены пароля
 *
 * storageType string
 * displayType PASSWORD
 */
class PXDisplayTypePassword extends PXDisplayType {
	function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) && mb_strlen($object[$field->name]) ? $object[$field->name] : NULL;
		return NLAbstractHTMLForm::BuildPassword($field->name, $value);
	}

	function buildCell($field, $object, $param=NULL) {
		$ret = '';
		if (mb_strlen($object[$field->name]) > 0) {
			$obj = new PXDisplayTypeCheckbox($this->display, $this->name); /* E_STRICT remove */
			$ret = $obj->buildCell($field, array($field->name => true), $param);
		}
		return $ret;
	}

	function getFromRequest($field, $object, $param=NULL) {
		$tmp = $param['request']->postData->_GetArrayVar($field->name);

		$value = (isset($tmp['type']) && isset($tmp['retype']) && mb_strlen($tmp['type']) && ($tmp['type'] === $tmp['retype'])) ? trim($tmp['type']) : null;
		if(mb_strlen($value) && !is_null($value)) {

			$authDriver = PXRegistry::getUser()
				->getPrimaryAuthMethod(empty($field->displayTypeArgs[0]) ? null : $field->displayTypeArgs[0]);

			if (!is_callable([$authDriver, 'passwdToDB'])) {
				FatalError("Can't process password with {$authDriver}::passwdToDB() !");
			}

			$value = call_user_func(array($authDriver, 'passwdToDB'), $value);
		}

		if(isset($tmp['delete']) && $tmp['delete'] === 'on') {
			$value = "\n\t\n";
		}

		return $value;
	}

	function preModifyObject($field, &$object, $param=NULL) {
		if(!mb_strlen($object[$field->name])) {
			return true;
		}

		if($object[$field->name] === "\n\t\n") {
			$object[$field->name] = null;
		}
	}
}

