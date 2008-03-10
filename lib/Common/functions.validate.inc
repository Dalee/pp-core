<?
function isEmail($string) {
	if (!preg_match("/^[a-z0-9_\-\.]{1,128}@(([a-z0-9-]+\.)+(com|net|org|mil|edu|gov|arpa|info|biz|inc|name|[a-z]{2})|[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3})$/is", $string)) {
		return FALSE;
	} else {
		return TRUE;
	}
}

function isValidEmail($string) {
	return isEmail($string);
}

function isValidLogin($string, $min=2, $max=16) {
	if (!preg_match('/^[a-z0-9_\-\.]{'.$min.','.$max.'}$/is', $string)) {
		return FALSE;
	} else {
		return TRUE;
	}
}

function isValidPasswd($string, $min=3, $max=16) {
	if (!preg_match('/^[a-z0-9_\-\.]{'.$min.','.$max.'}$/is', $string)) {
		return FALSE;
	} else {
		return TRUE;
	}
}

function isValidString($string, $min=1, $max=1024) {
	if (!preg_match('/^.{'.$min.','.$max.'}$/is', $string)) {
		return FALSE;
	} else {
		return TRUE;
	}
}

?>
