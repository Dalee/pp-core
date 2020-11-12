<?php
/**
 * Целочисленный массив
 * В базе может храниться как integer[]
 * Для плоских одномерных массивов рекомендуется более эффективный PXStorageTypeFlatIntArray
 *
 * displayType HIDDEN, STATIC, TABLE
 * storageType intarray
 */
class PXStorageTypeIntArray extends PXStorageTypeArray
{

	public const defaultSQLType = 'INTEGER[]';

	protected static $brackets = [['{', '}'], ['[', ']']];

	public function valueToDB($k, $v)
	{
		return ($v === '' || is_null($v)) ? 'null' : (int)$v;
	}

	public function normalize($field, $object, &$param)
	{
		$value = $object[$field->name];
		if (empty($value)) { // checking for empty value
			return [];
		}

		$value = strtolower($value); // avoid json_decode error in case with NULL values, transform NULL -> null
		$value = $this->prepareValue($value);

		// protect from json_decode errors
		return ($array = json_decode($value)) === null ? parent::normalize($field, $object, $param) : $array;
	}

	protected function prepareValue($value)
	{
		return str_replace(self::$brackets[0], self::$brackets[1], $value);
	}

}

?>
