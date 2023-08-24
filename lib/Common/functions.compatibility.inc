<?php

// FIXME: remove from local and remove from here
function pp_simplexml_decode_string($value) {
	return (string)$value;
}

// FIXME: remove from local and remove from here
function pp_simplexml_encode_string($value) {
	return (string)$value;
}

// no function in PHP, simulating
if (! function_exists('mb_strcasecmp')) {
	function mb_strcasecmp($str1, $str2, $encoding = null) {
		if (null === $encoding) {

			$encoding = mb_internal_encoding();
		}
		return strcmp(mb_strtoupper((string) $str1, $encoding), mb_strtoupper((string) $str2, $encoding));
	}
}

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
		$diff = strlen((string) $input) - mb_strlen((string) $input);
		return str_pad((string) $input, $pad_length+$diff, $pad_string, $pad_type);
	}
}

if (! function_exists('mb_ucfirst')) {
	function mb_ucfirst($string) {
		$l = mb_substr((string) $string, 0, 1);
		$l = mb_strtoupper($l);
		return $l.mb_substr((string) $string, 1);
	}
}
if (! function_exists('mb_lcfirst')) {
	function mb_lcfirst($string) {
		$l = mb_substr((string) $string, 0, 1);
		$l = mb_strtolower($l);
		return $l.mb_substr((string) $string, 1);
	}
}
