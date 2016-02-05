<?php
/**
 * Сериализованный массив
 * В базе может храниться как TEXT
 *
 * displayType HIDDEN, STATIC, TABLE, LINKTOFILE
 * storageType yaml
 */

class PXStorageTypeYaml extends PXStorageType {
	function getFromRequest($field, $object, &$param) {
		$k = $field->name;
		return (is_array($object[$k])) ? $object[$k] : array();
	}

	function normalizeObjectAttribute($field, $object, &$param) {
		$k = $field->name;
		$v = self::toYaml(is_array($object[$k]) ? $object[$k] : array());

		if(is_array($param['dbFields'])) {
			$param['dbFields'][] = $k;
		}

		if(is_array($param['dbValues'])) {
			$param['dbValues'][] = $v;
		}

		return array($v, $param['dbFields'], $param['dbValues']);
	}

	static function toArray($yaml) {
		return PXYaml::parse($yaml);
	}

	static function toYaml($array) {
		return PXYaml::dump($array);
	}

	function normalize($field, $object, &$param) {
		return ($object[$field->name]) ? self::toArray($object[$field->name]) : $object[$field->name];
	}
}
?>
