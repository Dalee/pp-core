<?php
/**
 * Сериализованный массив
 * В базе может храниться как TEXT
 *
 * displayType HIDDEN, STATIC, TABLE, LINKTOFILE
 * storageType serialized
 */

class PXStorageTypeSerialized extends PXStorageType
{
	public function getFromRequest($field, $object, &$param)
	{
		$k = $field->name;
		return (is_array($object[$k])) ? $object[$k] : [];
	}

	public function normalizeObjectAttribute($field, $object, &$param)
	{
		$k = $field->name;
		$v = json_encode((is_array($object[$k])) ? $object[$k] : []);

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
		$k = $field->name;
		$w = $object[$k];
		$v = null;

		// skip empties and nulls
		if ($w && $w !== 'null') {
			$v = json_decode((string) $w, true);
		}

		return $v;
	}

}
