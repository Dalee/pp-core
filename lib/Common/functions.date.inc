<?
function DateGermanToUnix($time) {
	sscanf($time, "%d.%d.%d %d:%d:%d.%d", $day, $month, $year, $hour, $minute, $second, $milli);
	return mktime($hour, $minute, $second, $month, $day, $year);
}

?>
