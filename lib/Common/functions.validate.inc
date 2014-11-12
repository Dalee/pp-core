<?php
function isEmail($string) {
	$string = trim($string);
	$emails = preg_split('/\s*[,;]\s*/'.REGEX_MOD, $string);

	foreach ($emails as $email) {
		//since PHP 5.2.0
		if (!filter_var($email, FILTER_VALIDATE_EMAIL)) {
			return false;
		}
	}

	return true;
}

function isValidEmail($string) {
	return isEmail($string);
}

function isValidLogin($string, $min=2, $max=16) {
	return (bool) preg_match('/^[a-z0-9_\-\.]{'.$min.','.$max.'}$/is'.REGEX_MOD, $string);
}

function isValidPasswd($string, $min=3, $max=16) {
	$len = mb_strlen($string);
	return $min <= $len && $len <= $max && ctype_graph($string);
}

function isValidString($string, $min=1, $max=1024) {
	return (bool) preg_match('/^.{'.$min.','.$max.'}$/is'.REGEX_MOD, $string);
}

?>
