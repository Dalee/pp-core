<?php
/**
 * Число с плавающей запятой
 * В базе может храниться как REAL
 *
 * displayType HIDDEN, TEXT, STATIC
 * storageType float
 */
class PXStorageTypeFloat extends PXStorageType {

	const defaultSQLType = 'DECIMAL(16, 4)';

	function getFromRequest($field, $object, &$param) {
		$k = $field->name;
		return (!(is_null($object[$k]) || $object[$k] === '')) ? parseFloat($object[$k]) : NULL;
	}

	function objectsSortOrder($field, $object, &$param = NULL) { /* E_STRICT */
		return $this->objectsSortOrderString($field, $object, $param);
	}

	function normalizeObjectAttribute($field, $object, &$param) {
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