<?php
function isEmail($string) {
	$string = trim($string);
	$emails = preg_split('/\s*[,;]\s*/', $string);

	foreach($emails as $string) {
		if (!preg_match("/^[a-z0-9_\-\.]{1,128}@(([a-z0-9-]+\.)+(com|net|org|mil|edu|gov|arpa|info|biz|inc|name|mobi|[a-z]{2})|[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3})$/is", $string)) {
			return false;
		}
	}

	return true;
}

function isValidEmail($string) {
	return isEmail($string);
}

function isValidLogin($string, $min=2, $max=16) {
	return (bool) preg_match('/^[a-z0-9_\-\.]{'.$min.','.$max.'}$/is', $string);
}

function isValidPasswd($string, $min=3, $max=16) {
	return (bool) preg_match('/^[a-z0-9_\-\.]{'.$min.','.$max.'}$/is', $string);
}

function isValidString($string, $min=1, $max=1024) {
	return (bool) preg_match('/^.{'.$min.','.$max.'}$/is', $string);
}

?>
