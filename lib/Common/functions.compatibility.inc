<?php

// no function, but strtr is most cases are utf-8 safe
if (! function_exists('mb_strtr')) {
	function mb_strtr($string, $from, $to = null) {
		if (is_array($from)) {
			return strtr($string, $from);
		}
		return strtr($string, $from, $to);
	}
}

if (! function_exists('mb_str_pad')) {
	function mb_str_pad($input, $pad_length, $pad_string=' ', $pad_type=STR_PAD_RIGHT) {
		$diff = strlen($input) - mb_strlen($input);
		return str_pad($input, $pad_length+$diff, $pad_string, $pad_type);
	}
}

if (! function_exists('mb_ucfirst')) {
	function mb_ucfirst($string) {
		$l = mb_substr($string, 0, 1);
		$l = mb_strtoupper($l);
		return $l.mb_substr($string, 1);
	}
}

// mbstring compatibility layer
if (! defined('MB_OVERLOAD_STRING')) {
	define('MB_DISABLED', true);

	function mb_substr() {
		$a = func_get_args();
		return call_user_func_array('substr', $a);
	}

	function mb_strpos() {
		$a = func_get_args();
		return call_user_func_array('strpos', $a);
	}

	function mb_stripos() {
		$a = func_get_args();
		return call_user_func_array('stripos', $a);
	}

	function mb_strrpos() {
		$a = func_get_args();
		return call_user_func_array('mb_strrpos', $a);		
	}

	function mb_strlen() {
		$a = func_get_args();
		$a = array_slice($a, 0, 1);
		return call_user_func_array('strlen', $a);
	}

	function mb_strtoupper() {
		$a = func_get_args();
		$a = array_slice($a, 0, 1);
		return call_user_func_array('strtoupper', $a);
	}

	function mb_strtolower() {
		$a = func_get_args();
		$a = array_slice($a, 0, 1);
		return call_user_func_array('strtolower', $a);
	}

	function mb_parse_str() {
		$a = func_get_args();
		return call_user_func_array('parse_str', $a);
	}

}
