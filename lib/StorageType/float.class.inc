<?php
/**
 * Число с плавающей запятой
 * В базе может храниться как REAL
 *
 * displayType HIDDEN, TEXT, STATIC
 * storageType float
 */
class PXStorageTypeFloat extends PXStorageType
{

	public const defaultSQLType = 'DECIMAL(16, 4)';

	public function getFromRequest($field, $object, &$param)
	{
		$k = $field->name;
		return (!(is_null($object[$k]) || $object[$k] === '')) ? parseFloat($object[$k]) : NULL;
	}

	public function objectsSortOrder($field, $object, &$param = NULL)
	{ /* E_STRICT */
		return $this->objectsSortOrderString($field, $object, $param);
	}

	public function normalizeObjectAttribute($field, $object, &$param)
	{
		$k = $field->name;

		$v = parseFloat($object[$k]);
		$v = $param['db']->exportFloat($v);

		if (is_array($param['dbFields'])) {
			$param['dbFields'][] = $k;
		}

		if (is_array($param['dbValues'])) {
			$param['dbValues'][] = $v;
		}

		return [$v, $param['dbFields'], $param['dbValues']];
	}
}
?>
