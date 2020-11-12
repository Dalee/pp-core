<?php

use Symfony\Component\Yaml\Yaml;

/**
 * Class PXStorageTypeYaml
 *
 * Сериализованный массив
 * В базе может храниться как TEXT
 *
 * displayType HIDDEN, STATIC, TABLE, LINKTOFILE
 * storageType yaml
 */
class PXStorageTypeYaml extends PXStorageType
{
	public function getFromRequest($field, $object, &$param)
	{
		$k = $field->name;
		return (is_array($object[$k])) ? $object[$k] : [];
	}

	public function normalizeObjectAttribute($field, $object, &$param)
	{
		$k = $field->name;
		$v = self::toYaml(is_array($object[$k]) ? $object[$k] : []);

		if (is_array($param['dbFields'])) {
			$param['dbFields'][] = $k;
		}

		if (is_array($param['dbValues'])) {
			$param['dbValues'][] = $v;
		}

		return [$v, $param['dbFields'], $param['dbValues']];
	}

	public static function toArray($yaml)
	{
		return Yaml::parse($yaml);
	}

	public static function toYaml($array)
	{
		return Yaml::dump($array);
	}

	public function normalize($field, $object, &$param)
	{
		return ($object[$field->name]) ? self::toArray($object[$field->name]) : $object[$field->name];
	}
}
