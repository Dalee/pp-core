<?php
/**
 * ���������� ������
 *
 * storageType ��� ������� integet, ������ string
 * displayType DROPDOWN|width
 *             width �� ������������
 */
class PXDisplayTypeDropdown extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) ? htmlspecialchars($object[$field->name]) : NULL;
		return NLAbstractHTMLForm::BuildDropDown($field->name, $value, $field->values->GetList());
	}

	static function buildCell($field, $object, $param=NULL) {
		$displayField = $field->values->displayField;
		$value = $object[$field->name];
		return isset($field->values->values[$value]) && isset($field->values->values[$value][$displayField]) ? $field->values->values[$value][$displayField] : '';
	}
}
?>