<?php
/**
 * Массив
 * В базе может храниться как integer[], string[]
 * Реализация проигрывает по скорости PXStorageTypeIntArray и PXStorageTypeFlatIntArray,
 * поэтому для integer[] не рекомендуется.
 *
 * displayType HIDDEN, STATIC, TABLE
 * storageType array
 */
class PXStorageTypeArray extends PXStorageType
{

	public const defaultSQLType = 'VARCHAR[]';

	public const STATE_BEGIN = 1;
	public const STATE_INARRAY = 2;
	public const STATE_OUTARRAY = 3;
	public const STATE_INSLASH = 4;
	public const STATE_INQUOTES = 5;

	public function getFromRequest($field, $object, &$param)
	{
		$k = $field->name;
		return (is_array($object[$k])) ? $object[$k] : [];
	}

	public function normalizeObjectAttribute($field, $object, &$param)
	{
		$value = $object[$field->name];
		$valueInDb = $this->arrayToDB($value);
		if (is_array($param['dbFields'])) {
			$param['dbFields'][] = $field->name;
		}

		if (is_array($param['dbValues'])) {
			$param['dbValues'][] = $valueInDb;
		}

		return [$valueInDb, $param['dbFields'], $param['dbValues']];
	}

	public function valueToDB($k, $v)
	{
		return sprintf('"%s"', str_replace('"', '\\\\"', PXRegistry::getDB()->EscapeString($v)));
	}

	public function arrayToDB(array $vals)
	{
		$res = [];
		foreach ($vals as $k => $v) {
			$res[] = is_array($v) ? $this->arrayToDB($v) : $this->valueToDB($k, $v);
		}
		return sprintf("{%s}", implode(",", $res));
	}

	public function normalize($field, $object, &$param)
	{
		$value = $object[$field->name];
		$resultArray = $indexArray = [];
		$level = $index = 0;
		$ptr = &$resultArray;
		for ($i = 0; $i < strlen((string) $value); $i++) {
			switch ($level) {
				case 1:
					if ($index > 0) {
						$ptr = &$ptr[sizeof($ptr)];
					}
					$indexArray[++$index] = &$ptr;
					break;
				case -1:
					$ptr = &$indexArray[--$index];
					break;
			}
			$level = $this->processFSM($value[$i], $ptr);
		}
		return $resultArray;
	}

	private function processFSM($chr, &$result)
	{
		static $state = self::STATE_BEGIN, $index = 0;
		$level = 0;
		switch (true) {
			case $chr == '{' && in_array($state, [self::STATE_BEGIN, self::STATE_INARRAY, self::STATE_OUTARRAY], true):
				$state = self::STATE_INARRAY;
				$index = 0;
				$level = +1;
				break;
			case $chr == '}' && in_array($state, [self::STATE_INARRAY, self::STATE_OUTARRAY], true):
				$state = self::STATE_OUTARRAY;
				$level = -1;
				break;
			case $chr == '\\' && $state !== self::STATE_BEGIN:
				$state = $state === self::STATE_INSLASH ? self::STATE_INQUOTES : self::STATE_INSLASH;
				break;

			case $chr == '"' && !in_array($state, [self::STATE_BEGIN, self::STATE_INSLASH], true):
				$state = $state === self::STATE_INQUOTES ? self::STATE_INARRAY : self::STATE_INQUOTES;
				break;

			case $chr == ',' && in_array($state, [self::STATE_INARRAY, self::STATE_OUTARRAY], true):
				$index = sizeof($result);
				break;

			case $state !== self::STATE_BEGIN:
				$state = $state === self::STATE_INSLASH ? self::STATE_INQUOTES : $state;
				isset($result[$index]) or $result[$index] = '';
				$result[$index] .= $chr;
				break;
		}
		return $level;
	}

}
