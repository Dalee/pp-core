<?php
/**
 * ��������� ����
 *
 * storageType string, integer ��� float
 * displayType TEXT|width|height
 *             width �� ������������
 *             ���� height = 0, �� �������� <input type="text">
 *             ���� height > 0, �� �������� <textarea> � height ��� �� ������ � ��������
 */
class PXDisplayTypeText extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) ? htmlspecialchars($object[$field->name]) : NULL;
		$width = isset($field->displayTypeArgs[1]) ? (int)$field->displayTypeArgs[1] : 0;
		return NLAbstractHTMLForm::BuildText($field->name, $value, $width);
	}

	static function buildCell($field, $object, $param=NULL) {
		$value = strip_tags($object[$field->name]);
		return strlen($value) > 50 ? substr($value, 0, 50).'&hellip;' : $value;
	}
}
?>