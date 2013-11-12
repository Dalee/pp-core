<?php
/**
 * Сериализованный массив
 * В базе может храниться как TEXT
 *
 * displayType HIDDEN, STATIC, TABLE, LINKTOFILE
 * storageType serialized
 */

class PXStorageTypeSerialized extends PXStorageType {
	function getFromRequest($field, $object, &$param) {
		$k = $field->name;
		return (is_array($object[$k])) ? $object[$k] : array();
	}

	function normalizeObjectAttribute($field, $object, &$param) {
		$k = $field->name;
		$v = json_encode_koi((is_array($object[$k])) ? $object[$k] : array());

		if (is_array($param['dbFields'])) {
			$param['dbFields'][] = $k;
		}

		if (is_array($param['dbValues'])) {
			$param['dbValues'][] = $v;
		}

		return array($v, $param['dbFields'], $param['dbValues']);
	}

	function normalize($field, $object, &$param) {
		$k = $field->name;
		$v = null;
		if ($object[$k]) {
			// NOTICE: no mb_* is correct here
			$sign = substr($object[$k], 0, 2);
			$v = ($sign === 'a:') ? unserialize($object[$k]) : json_decode_koi($object[$k]);
		}
		return $v;
	}
}
