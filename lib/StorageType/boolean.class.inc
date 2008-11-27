<?php
/**
 * Логическое поле, флаг
 * В базе может храниться как BOOL
 *
 * displayType HIDDEN, CHECKBOX, STATIC
 * storageType boolean
 */
class PXStorageTypeBoolean extends PXStorageType {
	static function getFromRequest($field, $object, &$param) {
		return (bool)$object[$field->name];
	}

	static function objectsSortOrder($field, $object, &$param) {
		return PXStorageTypeString::objectsSortOrder($field, $object, $param);
	}

	static function normalizeObjectAttribute($field, $object, &$param) {
		$k = $field->name;
		$v = (bool)$object[$k] ? '1' : '0';

		if(is_array($param['dbFields'])) {
			$param['dbFields'][] = $k;
		}

		if(is_array($param['dbValues'])) {
			$param['dbValues'][] = $v;
		}

		return array($v, $param['dbFields'], $param['dbValues']);
	}

	static function normalize($field, $object, &$param) {
		return $param['db']->importBoolean($object[$field->name]);
	}
}
?>