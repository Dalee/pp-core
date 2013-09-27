<?php
/**
 * Целое число
 * В базе может храниться как INT4
 *
 * displayType HIDDEN, TEXT, DROPDOWN, STATIC, PARENTDROPDOWN, SELFPARENTDROPDOWN, IPADDR
 * storageType integer
 */
class PXStorageTypeInteger extends PXStorageType {

	const defaultSQLType = 'INTEGER';

	function getFromRequest($field, $object, &$param) {
		$k = $field->name;
		return (!(is_null($object[$k]) || $object[$k] === '')) ? (int)$object[$k] : NULL;
	}

	function objectsSortOrder($field, $object, &$param = NULL) { /* E_STRICT */
		return $this->objectsSortOrderString($field, $object, $param);
	}

	function normalizeObjectAttribute($field, $object, &$param) {
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
}
?>