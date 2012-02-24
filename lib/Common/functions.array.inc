<?php

/**
 * Copy the N fields from the table rows.
 *
 * @param array $table
 * @param array $rows
 * @return array
 */
function GetColsFromTable($table, $rows) {
	$results = array();
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

function GetColsFromTableWithIndex($table, $rows, $index) {
	$results = array();
	if (!is_array($table)) {
		return $results;
	}
	$rows = array_flip($rows);
	foreach ($table as $id => $row) {
		if (is_array($row)) {
			$k = $index ? $row[$index] : $id;
			$results[] = array_intersect_key($row, $rows);
		}
	}
	return $results;
}

function GetBlankArray() {
	return array();
}

function GetColFromTable($table, $rowName) {
	$retArray = array();
	foreach ($table as $row) {
		if(isset($row[$rowName]) && !is_null($row[$rowName])) {
			$retArray[] = $row[$rowName];
		}
	}
	return $retArray;
}

//grouping hash by key
function groupArrayByCol($table, $rowNameOrCallBack) {
	$tmp = array();
	$is_callable = is_callable($rowNameOrCallBack);

	foreach($table as $k => $row) {
		$rowName = $is_callable
			? $rowNameOrCallBack($row)
			: $rowNameOrCallBack;

		if ($is_callable || isset($row[$rowName])) {
			$tmp[$is_callable ? $rowName : $row[$rowName]][] = $row;
		}
	}

	return $tmp;
}

function GetColFromTableWithIndexs($table, $rowName) {
	$retArray = array();
	foreach ($table as $index=>$row) {
		$retArray[$index] = $row[$rowName];
	}
	return $retArray;
}

function SortRussianByTitle($a, $b) {
	if ($a == $b) return 0;
	return (convert_cyr_string($a['title'], 'k', 'w') < convert_cyr_string($b['title'], 'k', 'w')) ? -1 : 1;
}


function GetFromArray($array, $index, $default=NULL) {
	if (isset($array[$index])) {
		return $array[$index];
	} else {
		return $default;
	}
}

function NumericCmp($a, $b) {
	if ($a == $b) return 0;
	return ($a < $b) ? -1 : 1;
}

//Thanks to the PHP.NET comments, it is very helpful function sometimes, but it exists only in PHP5
if(!function_exists('array_combine')){
	function array_combine($arr1, $arr2) {
		$out = array();
		
		$arr1 = array_values($arr1);
		$arr2 = array_values($arr2);
		
		foreach($arr1 as $key1 => $value1) {
			$out[(string)$value1] = $arr2[$key1];
		}
		
		return $out;
	}
}

function NestedArrayWalk(&$multiArray, $mapper, $optArgs = array(), $nestingLevel = 0){
	if(!is_callable($mapper)) {
		return false;
	}

	foreach($multiArray as $key => $value){
		if(is_array($value) && sizeof($value)){
			NestedArrayWalk($multiArray[$key], $mapper, $optArgs, (int)$nestingLevel + 1);
		}

		$mapper($multiArray, $key, $optArgs, $nestingLevel);
	}

	return true;
}

function array_recursive_inheritance() {
	$arrays = func_get_args();
	$base = array_shift($arrays);
	if(!is_array($base)) $base = empty($base) ? array() : array($base);
	foreach($arrays as $append) {
		foreach((array)$append as $key => $value) {
			if(!array_key_exists($key, $base)) {
				$base[$key] = $append[$key];
				continue;
			}
			if(is_array($value) or is_array($base[$key])) {
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
	$t = array();

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

	$result = array();

	if (is_string($map)) {
		$map = array($map => $map);
	}

	if (!$strict && isset($map[0])) {
		$rmap = array();
		foreach($map as $k=>$v) {
			$rmap[ (is_int($k) && isset($array[$v])) ? $v : $k ] = $v;
		}
		$map = $rmap;
	}

	foreach ($map as $k => $v) {
		$result[$k] = isset($array[$v]) ? $array[$v] : null;
	}

	return $result;
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
function array_flat($array, $toKey = false, $toValue = false) {

	if (!is_array($array)) {
		return false;
	}

	$result = array();

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
			// if scalar
			case is_scalar($toValue):
				$newValue = $value[$toValue];
		}

		// choose key
		switch (true) {
			// if we just simplifiying some kind of php array to list
			case $toKey === false:
				$result[] = $newValue;
				continue;
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

	foreach($inList as $element) {
		$pos++;
		if ($strict ? $current === $element : $current == $element) {
			$found = true;
			break;
		}
	}

	$prev = null;
	$next = null;

	if($found){
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
 * @param mixed $colValue	Значение ячейки в этом столбце
 * @param bool $strict	Использовать строгое (===) или нестрогое (==) сравнение
 * @param int $count	Максимальное количество возврщаемых строк
 * 
 * @return array	Таблица из строк, для которых в заданном столбце ячейка имеет заданное значение. Если ничего не найдено, то пустой массив.
 */
function getRowsFromTable($array, $colName, $colValue, $strict = true, $count = INF) {
	$found  = array();
	$nFound = 0;
	foreach($array as $idx => $row) {
		if (!array_key_exists($colName, $row) || ($strict ? ($row[$colName] !== $colValue) : ($row[$colName] != $colValue))) {
			continue;
		}
		$found[$idx] = $row;
		if(++$nFound >= $count){
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
* @param mixed $colValue	Значение ячейки в этом столбце
* @param bool $strict	Использовать строгое (===) или нестрогое (==) сравнение
*
* @return array	Строка, для которой в заданном столбце ячейка имеет заданное значение, либо пустой массив в случае, если ничего не найдено.
*/
function getRowFromTable($array, $colName, $colValue, $strict = true) {
	$found = getRowsFromTable($array, $colName, $colValue, $strict, 1);
	count($found) && $found = reset($found);
	return $found;
}

?>
