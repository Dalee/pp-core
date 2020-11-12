<?php
/**
 * Целое число
 * В базе может храниться как INT4
 *
 * displayType HIDDEN, TEXT, DROPDOWN, STATIC, PARENTDROPDOWN, SELFPARENTDROPDOWN, IPADDR
 * storageType integer
 */
class PXStorageTypeInteger extends PXStorageType
{

	public const defaultSQLType = 'INTEGER';

	public function getFromRequest($field, $object, &$param)
	{
		$k = $field->name;
		return (!(is_null($object[$k]) || $object[$k] === '')) ? (int)$object[$k] : NULL;
	}

	public function objectsSortOrder($field, $object, &$param = NULL)
	{ /* E_STRICT */
		return $this->objectsSortOrderString($field, $object, $param);
	}

	public function normalizeObjectAttribute($field, $object, &$param)
	{
		$k = $field->name;

		$v = (is_numeric($object[$k]) || empty($object[$k])) ? $object[$k] : 0;

		if (is_array($param['dbFields'])) {
			$param['dbFields'][] = $k;
		}

		if (is_array($param['dbValues'])) {
			$param['dbValues'][] = $v;
		}

		return [$v, $param['dbFields'], $param['dbValues']];
	}

	public function filterable()
	{
		return true;
	}
}
?>
