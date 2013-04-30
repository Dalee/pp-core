<?php
/**
 * Нередактируемое поле
 *
 * storageType может быть любым, но как правило integer или string
 * displayType STATIC
 */
class PXDisplayTypeStatic extends PXDisplayType {
	function buildInput($field, $object, $param=NULL) {
		$k = $field->name;
		$value = isset($object[$k]) ? htmlspecialchars(is_array($object[$k]) ? print_r($object[$k], true) : $object[$k], ENT_COMPAT|ENT_HTML401, DEFAULT_CHARSET) : NULL;

		$values = isset($field->values) ? $field->values->GetList() : array();
		return NLAbstractHTMLForm::BuildHidden($field->name, $value).(isset($values[$value]) ? $values[$value] : $value);
	}

	function buildCell($field, $object, $param=NULL) {
		$value = $object[$field->name];
		$value =  isset($field->values->values[$value]) && isset($field->values->values[$value]['title']) ? $field->values->values[$value]['title'] : $value;
		
		return $this->getSubDirectoryIfExists($field, $object, $value);
	}

	function preModifyObject($field, &$object, $param=NULL) {
		return (bool)$param['skipHidden'];
	}
}
?>
