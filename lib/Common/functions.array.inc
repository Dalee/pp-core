<?php

/**
 * Copy the N fields from the table rows.
 *
 * @param array $table
 * @param array $rows
 * @return array
 */
function getColsFromTable($table, $rows) {
	$results = [];
	if (!is_array($table)) {
		return $results;
	}
	$rows = array_flip($rows);
	foreach ($table as $row) {
		if (is_array($row)) {
			$results[] = array_intersect_key($row, $rows);
		}
	}
	return $results;
}

function getColsFromTableWithIndex($table, $rows, $index) {
	$results = array();
	if (!is_array($table)) {
		return $results;
	}
	$rows = array_flip($rows);
	foreach ($table as $id => $row) {
		if (is_array($row)) {
			$k = $index ? $row[$index] : $id;
			$results[$k] = array_intersect_key($row, $rows);
		}
	}
	return $results;
}

function getBlankArray() {
	return [];
}

function getColFromTable($table, $rowName) {
	$retArray = [];
	foreach ($table as $row) {
		if (isset($row[$rowName]) && !is_null($row[$rowName])) {
			$retArray[] = $row[$rowName];
		}
	}
	return $retArray;
}

// grouping hash by key
function groupArrayByCol($table, $rowNameOrCallBack, $allowEmptyKeys = false, $fixCallableRowName = false) {
	$tmp = array();
	$is_callable = !$fixCallableRowName && is_callable($rowNameOrCallBack);

	foreach ($table as $k => $row) {
		$rowName = $is_callable
			? $rowNameOrCallBack($row)
			: $rowNameOrCallBack;

		if ($is_callable || ($allowEmptyKeys ? array_key_exists($rowName, $row) : isset($row[$rowName]))) {
			$tmp[$is_callable ? $rowName : $row[$rowName]][] = $row;
		}
	}

	return $tmp;
}

function getColFromTableWithIndexs($table, $rowName) {
	$retArray = [];
	foreach ($table as $index => $row) {
		$retArray[$index] = $row[$rowName];
	}
	return $retArray;
}

/**
 * Gets value from array by key if it exists
 * or $default otherwise
 *
 * @param array $array
 * @param string $index
 * @param null $default
 * @return mixed
 */
function getFromArray($array, $index, $default = null) {
	if (isset($array[$index])) {
		return $array[$index];
	} else {
		return $default;
	}
}

function nestedArrayWalk(&$multiArray, $mapper, $optArgs = [], $nestingLevel = 0) {
	if (!is_callable($mapper)) {
		return false;
	}

	foreach ($multiArray as $key => $value) {
		if (is_array($value) && sizeof($value)) {
			nestedArrayWalk($multiArray[$key], $mapper, $optArgs, (int)$nestingLevel + 1);
		}

		$mapper($multiArray, $key, $optArgs, $nestingLevel);
	}

	return true;
}

function array_recursive_inheritance() {
	$arrays = func_get_args();
	$base = array_shift($arrays);
	if (!is_array($base)) $base = empty($base) ? array() : array($base);
	foreach ($arrays as $append) {
		foreach ((array)$append as $key => $value) {
			if (!array_key_exists($key, $base)) {
				$base[$key] = $append[$key];
				continue;
			}
			if (is_array($value) or is_array($base[$key])) {
				$base[$key] = array_recursive_inheritance($base[$key], $append[$key]);
			} else {
				$base[$key] = $value;
			}
		}
	}
	return $base;
}

/**
 * Simple transpose operation
 * Useful for tables (arrays of maps with the same fields)
 *
 * @param array $array 2-dim array
 * @return array transposed 2-dim array
 */
function array_transpose($array) {
	$t = [];

	foreach ($array as $key => $value) {
		foreach ($value as $key2 => $value2) {
			$t[$key2][$key] = $value2;
		}
	}

	return $t;
}

/**
 * Assoc array fields rename
 *
 * @param array $array existing array
 * @param mixed $map existing keys or/and their new names
 * @param boolean $strict
 * @return array resulting array with selected keys
 */
function array_remap($array, $map, $strict = false) {
	if (!is_string($map) && !is_array($map)) {
		return false;
	}

	$result = [];

	if (is_string($map)) {
		$map = array($map => $map);
	}

	if (!$strict && isset($map[0])) {
		$rmap = array();
		foreach ($map as $k=>$v) {
			$rmap[ (is_int($k) && isset($array[$v])) ? $v : $k ] = $v;
		}
		$map = $rmap;
	}

	foreach ($map as $k => $v) {
		$result[$k] = isset($array[$v]) ? $array[$v] : null;
	}

	return $result;
}

function array_deep_diff($d1, $d2, $excludeKeys) {
	switch (true) {
		case is_array($d1) && is_array($d2) :
			$diff = array();
			foreach (array_unique(array_merge(array_keys($d1), array_keys($d2))) as $key) {
				if (is_array($excludeKeys) && in_array($key, $excludeKeys)) {
					continue;
				}

				if (!array_key_exists($key, $d1)) {
					$diff['added'][$key] = $d2[$key];
				} else if (!array_key_exists($key, $d2)) {
					$diff['removed'][$key] = $d1[$key];
				} else {
					$tmp = array_deep_diff($d1[$key], $d2[$key], $excludeKeys);
					if (!empty($tmp)) $diff[$key] = $tmp;
				}
			}
			return $diff;
		case (is_scalar($d1) || is_null($d1)) && (is_scalar($d2) || is_null($d2)) :
			if ($d1 == $d2) {
				return array();
			}
			$ret = "$d1 ---> $d2";
			// just a little touch for numerics, might not be needed
			if (is_numeric($d1) && is_numeric($d2)) {
				if ($d1 > $d2) $ret .= ' [- ' . ($d1 - $d2) . ']';
				if ($d1 < $d2) $ret .= ' [+ ' . ($d2 - $d1) . ']';
			}
			return $ret;
		case is_object($d1) && is_object($d2) :
			/* Два объекта нельзя сравнивать через == , т.к. можно нарваться на циклические зависимости и
			 * в результате получить FatalError. А сравнивать через === бессмысленно. */
			return array();
		default:
			/* скаляр vs массив, массив vs объект, скаляр vs объект, ресурс vs что угодно,
			 * колбэки vs что угодно */
			return array('Noncomparable values');
	}
}


/**
 * Array flatter (maker)
 * Note: boolean values is special. true is a $key, false is a current value ($key for key, $value for value)
 *
 * @param array $array source array
 * @param string $toKey a key existing keys or/and their new names
 * @param mixed $toValue a key or a list of existing keys in source array
 * @return array resulting array with keys $toKey and values $toValue
 * @example
 *   $arr = array(
 *     'x' => array('a' => 'foo', 'b' => 5, 'c' => 'oof'),
 *     'y' => array('a' => 'bar', 'b' => 2, 'c' => 'zzz'),
 *     'z' => array('a' => 'baz', 'b' => 6, 'c' => '123'),
 *     ...
 *   );
 *   array_flat($arr, 'a', 'b'); // array( 'foo' => 5, 'bar' => 2, 'baz' => 6, ... )
 *   array_flat($arr, 'c', true); // array( 'oof' => 'x', 'zzz' => 'y', '123' => 'z', ... )
 *   array_flat($arr, false, array('a', 'c')); // array( array('a' => 'foo', 'c' => 'oof'), array('a' => 'bar', 'c' => 'zzz'), ... )
 */
function array_flat($array, $toKey = false, $toValue = false, $unique = false) {

	if (!is_array($array)) {
		return false;
	}

	$result = [];

	foreach ($array as $key => $value) {

		// choose value
		switch (true) {
			// if list of keys
			case is_array($toValue):
				$newValue = array_intersect_key($value, array_flip(array_values($toValue)));
				break;
			// if false
			default:
			case $toValue === false:
				$newValue = $value; // as was
				break;
			// if true
			case $toValue === true:
				$newValue = $key; // as key
				break;
			// if dot delimited string keys
			case is_scalar($toValue) && strpos($toValue, '.'):
				$newValue = $value;
				foreach (explode('.', $toValue) as $toValueKey) {
					$newValue = $newValue[$toValueKey];
				}
				break;
			// if scalar
			case is_scalar($toValue):
				$newValue = $value[$toValue];
		}

		// choose key
		switch (true) {
			// if we just simplifiying some kind of php array to list
			case $toKey === false:
				$result[] = $newValue;
				continue 2;
			// default
			default:
			case $toKey === true:
			case is_scalar($value):
				$newKey = $key;
				break;
			// some field of value will be used as key in new map
			case is_array($value) && array_key_exists($toKey, $value):
				$newKey = $value[$toKey];
		}

		// push key-value pair to result
		$result[$newKey] = $newValue;
	}

	$unique && $result = array_flip(array_flip($result));

	return $result;
}

/**
 * Функция собирает в массивы ключи по значениям
 * Например, из [a=>1, b=>1, c=>2, d=>3, e=>2] функция вернет [1=>[a,b], 2=>[c,e], 3=>[d]]
 */
function array_keys_grouped($array) {
	$result = array();
	foreach (array_unique($array) as $v) {
		$result[$v] = array_keys($array, $v);
	}
	return $result;
}

/**
*
* Функция находит для первого встреченного заданного элемента предыдущий и последующий элементы в списке.
* @param array	$list		Список, в котором производится поиск. Передаётся по ссылке - иначе после foreach'а сбразывается указатель массива.
* @param mixed	$current	Сосед искомых элементов.
* @param bool	$circular	Считать ли последний элемент соседом первого.
* @param bool	$strict		Использовать строгое (true) или нестрогое (false) сравнение при поиске элемента.
* @return array Список из двух элементов - предыдущего и последующего.
*/
function findNeighborElements($list, $current, $circular = true, $strict = true) {

	$inList = &$list; // Иначе foreach не меняет указатель массива по завершению, что ломает весь алгоритм

	$found = false;
	$pos   = 0;

	foreach ($inList as $element) {
		$pos++;
		if ($strict ? $current === $element : $current == $element) {
			$found = true;
			break;
		}
	}

	$prev = null;
	$next = null;

	if ($found) {
		$last = $pos + 1 > sizeof($inList);
		$last ? end($inList) : prev($inList);
		$last || $next = next($inList);
		$last || prev($inList);

		$first = $pos == 1;
		$first || $prev = prev($inList);

		$first && $circular	&& $prev = end($inList);
		$last  && $circular	&& $next = reset($inList);
	}
	return array($prev, $next);
}

/**
 * Поиск строк в таблице по значению одного из столбцов
 *
 * @param array $array	Таблица
 * @param string $colName	Название столбца
 * @param mixed $colValues	Значение ячейки в этом столбце, можно передавать список значений для проверки
 * @param bool $strict	Использовать строгое (===) или нестрогое (==) сравнение
 * @param int $count	Максимальное количество возврщаемых строк
 *
 * @return array	Таблица из строк, для которых в заданном столбце ячейка имеет заданное значение. Если ничего не найдено, то пустой массив.
 */
function getRowsFromTable($array, $colName, $colValues, $strict = true, $count = INF) {
	$found     = array();
	$nFound    = 0;
	$colValues = (array)$colValues;
	foreach ($array as $idx => $row) {
		//FIXME: change in_array() to something more effective, it will be slow for big lists
		if (!array_key_exists($colName, $row) || !in_array($row[$colName], $colValues, $strict)) {
			continue;
		}
		$found[$idx] = $row;
		if (++$nFound >= $count) {
			break;
		}
	}
	return $found;
}

/**
* Поиск строк в таблице по значению одного из столбцов
*
* @param array $array	Таблица
* @param string $colName	Название столбца
* @param mixed $colValues	Значение ячейки в этом столбце, можно передавать список значений для проверки
* @param bool $strict	Использовать строгое (===) или нестрогое (==) сравнение
*
* @return array	Строка, для которой в заданном столбце ячейка имеет заданное значение, либо пустой массив в случае, если ничего не найдено.
*/
function getRowFromTable($array, $colName, $colValues, $strict = true) {
	$found = getRowsFromTable($array, $colName, $colValues, $strict, 1);
	count($found) && $found = reset($found);
	return $found;
}

/**
 * Checks an array to be a simple array
 *
 * @param $array
 * @return bool|null
 */
function is_list($array) {
	if (empty($array) || !is_array($array)) {
		return null;
	}

	$l = sizeof($array) - 1;
	// first and last are numeric
	if (!array_key_exists(0, $array) || !array_key_exists($l, $array)) {
		return false;
	}

	for ($i = 1; $i < $l; $i++) {
		if (!array_key_exists($i, $array)) {
			return false;
		}
	}

	return true;
}

/**
 * Readable alias to "is_array() && !is_list()"
 *
 * @param array $array
 * @return bool|null
 */
function is_hash($array) {
	if (empty($array) || !is_array($array)) {
		return null;
	}

	return !is_list($array);
}
