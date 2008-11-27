<?php
/**
 * "�����������" ����
 * ����� �� ��������, �������� ������� ��� �� �������� �� ��������� ($field->defaultValue)
 * ��� ����� ����������� � PXOutputHandler'��
 *
 * displayType STATIC
 * storageType virtual
 */
class PXStorageTypeVirtual extends PXStorageType {
	static function normalize($field, $object, &$param) {
		return (!is_null($field->defaultValue)) ? $field->defaultValue : NULL;
	}

	static function notInDb($field, $object, $param=NULL) {
		return true;
	}
}

?>