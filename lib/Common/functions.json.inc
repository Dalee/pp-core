<?php

function __json_encode_koi_k2u(&$value) {
	$value = myconv(DEFAULT_CHARSET, CHARSET_UTF8, $value);
}
function __json_encode_koi_u2k(&$value) {
	$value = myconv(CHARSET_UTF8, DEFAULT_CHARSET, $value);
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

	switch(PHP_MINOR_VERSION) {
		case 2:
			$value = json_encode($value);
			break;
		case 3:
		case 4:
			$value = json_encode($value, $options);
			break;
	}

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
	switch(PHP_MINOR_VERSION) {
		case 2:
			$value = json_decode($value, $assoc);
			break;
		case 3:
			$value = json_decode($value, $assoc, $depth);
			break;
		case 4:
			$value = json_decode($value, $assoc, $depth, $options);
			break;
	}
	
	if (empty($value)) {
		// dummy
	} elseif (is_scalar($value)) {
		__json_encode_koi_u2k($value);
	} else {
		array_walk_recursive($value, '__json_encode_koi_u2k');
	}
	return $value;
}
