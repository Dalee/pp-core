<?php
function DateGermanToUnix($time) {
	sscanf($time, "%d.%d.%d %d:%d:%d.%d", $day, $month, $year, $hour, $minute, $second, $milli);
	return mktime($hour, $minute, $second, $month, $day, $year);
}

function DateUnixToGMT($timestamp = null){
	return str_replace('+0000', 'GMT', isset($timestamp) ? gmdate('r', $timestamp) : gmdate('r'));
}
?>