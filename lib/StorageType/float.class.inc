<?php
/**
 * Число с плавающей запятой
 * В базе может храниться как REAL
 *
 * displayType HIDDEN, TEXT, STATIC
 * storageType float
 */
class PXStorageTypeFloat extends PXStorageType {
	static function getFromRequest($field, $object, &$param) {
		$k = $field->name;
		return (!(is_null($object[$k]) || $object[$k] === '')) ? parseFloat($object[$k]) : NULL;
	}

	static function objectsSortOrder($field, $object, &$param) {
		return PXStorageTypeString::objectsSortOrder($field, $object, $param);
	}

	static function normalizeObjectAttribute($field, $object, &$param) {
		$k = $field->name;

		$v = parseFloat($object[$k]);
		$v = $param['db']->exportFloat($v);

		if(is_array($param['dbFields'])) {
			$param['dbFields'][] = $k;
		}

		if(is_array($param['dbValues'])) {
			$param['dbValues'][] = $v;
		}

		return array($v, $param['dbFields'], $param['dbValues']);
	}
}
?>
