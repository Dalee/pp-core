<?php
/**
 * Целое число, id объекта
 * В базе может храниться как INT4 PRIMARY KEY
 *
 * displayType HIDDEN, STATIC
 * storageType pk
 */
class PXStorageTypePk extends PXStorageTypeInteger {
	static function objectsSortOrder($field, $object, &$param) {
		return PXStorageType::objectsSortOrder($field, $object, $param);
	}

	static function normalizeObjectAttribute($field, $object, &$param) {
		return PXStorageType::normalizeObjectAttribute($field, $object, $param);
	}
}
?>
