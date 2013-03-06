<?php

function __json_encode_koi_k2u(&$value) {
	$value = myconv('k', 'u', $value);
}
function __json_encode_koi_u2k(&$value) {
	$value = myconv('u', 'k', $value);
}

/**
 * json_encode with koi8r-to-utf8 autoconversion
 * @param mixed $value native variable
 * @param integer $options since 5.3, 0 by default
 * @return string
 */
function json_encode_koi($value, $options = 0) {
	if (empty($value)) {
		// dummy
	} elseif (is_scalar($value)) {
		__json_encode_koi_k2u($value);
	} else {
		array_walk_recursive($value, '__json_encode_koi_k2u');
	}
	$value = PHP_MINOR_VERSION > 2
		? json_encode($value, $options)
		: json_encode($value);
	return $value;
}

/**
 * json_decode with utf8-to-koi8r autoconversion
 * @param string $value JSON encoded string
 * @param boolean $assoc false by default
 * @param integer $depth 512 by default
 * @param integer $options since 5.4, 0 by default
 * @return string
 */
function json_decode_koi($value, $assoc = false, $depth = 512, $options = 0) {
	$value = json_decode($value, $assoc, $depth, $options);
	if (is_scalar($value)) {
		__json_encode_koi_u2k($value);
	} else {
		array_walk_recursive($value, '__json_encode_koi_u2k');
	}
	return $value;
}
