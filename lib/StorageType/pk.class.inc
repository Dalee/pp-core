<?php
/**
 * Целое число, id объекта
 * В базе может храниться как INT4 PRIMARY KEY
 *
 * displayType HIDDEN, STATIC
 * storageType pk
 */
class PXStorageTypePk extends PXStorageTypeInteger {

	const defaultSQLType = 'SERIAL PRIMARY KEY';

	function objectsSortOrder($field, $object, &$param = NULL) { /* E_STRICT */
		return PXStorageType::objectsSortOrder($field, $object, $param);
	}

	function normalizeObjectAttribute($field, $object, &$param) {
		return PXStorageType::normalizeObjectAttribute($field, $object, $param);
	}
}
?>