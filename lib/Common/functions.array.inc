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
	if (!is_string($map) && !is_array($map))
		return false;
	
	$r = array();
	
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
	
	foreach ($map as $k => $v)
		$r[$k] = isset($array[$v]) ? $array[$v] : null;
	
	return $r;
}

?>
