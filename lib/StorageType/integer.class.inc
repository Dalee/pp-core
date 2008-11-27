<?php
/**
 * Целое число
 * В базе может храниться как INT4
 *
 * displayType HIDDEN, TEXT, DROPDOWN, STATIC, PARENTDROPDOWN, SELFPARENTDROPDOWN, IPADDR
 * storageType integer
 */
class PXStorageTypeInteger extends PXStorageType {
	static function getFromRequest($field, $object, &$param) {
		$k = $field->name;
		return (!(is_null($object[$k]) || $object[$k] === '')) ? (int)$object[$k] : NULL;
	}

	static function objectsSortOrder($field, $object, &$param) {
		return PXStorageTypeString::objectsSortOrder($field, $object, $param);
	}

	static function normalizeObjectAttribute($field, $object, &$param) {
		$k = $field->name;

		$v = (is_numeric($object[$k]) || empty($object[$k])) ? $object[$k] : 0;

		if(is_array($param['dbFields'])) {
			$param['dbFields'][] = $k;
		}

		if(is_array($param['dbValues'])) {
			$param['dbValues'][] = $v;
		}

		return array($v, $param['dbFields'], $param['dbValues']);
	}
}?>