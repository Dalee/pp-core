<?
function GetColFromTable($table, $rowName) {
	$retArray = array();
	foreach ($table as $row) {
		if(isset($row[$rowName]) && !is_null($row[$rowName])) {
			$retArray[] = $row[$rowName];
		}
	}
	return $retArray;
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


function GetFromArray(&$array, $index, $default=NULL) {
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

?>
