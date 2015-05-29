<?php
/**
 * JSON объект
 * В базе может храниться как TEXT или JSON (последние версии psql)
 *
 * displayType HIDDEN, STATIC, TABLE
 * storageType json
 */

class PXStorageTypeJson extends PXStorageType {

	function getFromRequest($field, $object, &$param) {
		$k = $field->name;
		return (is_array($object[$k])) ? $object[$k] : array();
	}

	function normalizeObjectAttribute($field, $object, &$param) {
		$k = $field->name;

		$v = json_encode((is_array($object[$k]) ? $object[$k] : array()));

		if (is_array($param['dbFields'])) {
			$param['dbFields'][] = $k;
		}
		if (is_array($param['dbValues'])) {
			$param['dbValues'][] = $v;
		}

		return array($v, $param['dbFields'], $param['dbValues']);
	}

	function normalize($field, $object, &$param) {
		return ($object[$field->name])? json_decode($object[$field->name], 1) : $object[$field->name];
	}
}
