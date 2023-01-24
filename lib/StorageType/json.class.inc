<?php
/**
 * JSON объект
 * В базе может храниться как TEXT или JSON (последние версии psql)
 *
 * displayType HIDDEN, STATIC, TABLE
 * storageType json
 */

class PXStorageTypeJson extends PXStorageType
{

	public function getFromRequest($field, $object, &$param)
	{
		$k = $field->name;
		return (is_array($object[$k])) ? $object[$k] : [];
	}

	public function normalizeObjectAttribute($field, $object, &$param)
	{
		$k = $field->name;

		$v = json_encode((is_array($object[$k]) ? $object[$k] : []), JSON_THROW_ON_ERROR);

		if (is_array($param['dbFields'])) {
			$param['dbFields'][] = $k;
		}
		if (is_array($param['dbValues'])) {
			$param['dbValues'][] = $v;
		}

		return [$v, $param['dbFields'], $param['dbValues']];
	}

	public function normalize($field, $object, &$param)
	{
		return ($object[$field->name]) ? json_decode((string) $object[$field->name], 1, 512, JSON_THROW_ON_ERROR) : $object[$field->name];
	}
}
