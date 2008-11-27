<?php
/**
 * ������� ����
 *
 * storageType ����� ���� �����, �� ��� ������� integer
 * displayType HIDDEN
 */
class PXDisplayTypeHidden extends PXDisplayType {
	static function buildRow($field, $object, $param=NULL) {
		$value = (isset($object[$field->name]) && !is_array($object[$field->name])) ? htmlspecialchars($object[$field->name]) : NULL;
		return NLAbstractHTMLForm::BuildHidden($field->name, $value);
	}

	static function getRow($even) {
		return $even;
	}

	static function preModifyObject($field, $object, $param=NULL) {
		return (bool)$param['preserveHidden'];
	}

	static function buildCell($field, $object, $param=NULL) {
		$value = is_string($object[$field->name]) ? strip_tags($object[$field->name]) : $object[$field->name];
		return $value;
	}
}
?>