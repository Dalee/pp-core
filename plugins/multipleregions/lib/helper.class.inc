<?php

class PXMultipleRegionsHelper {
	static function toArray($regString) {
		return array_filter(explode(",", strtr($regString, array("{" => "", "}" => ""))), "is_numeric");
	}

	static function toString($regArray) {
		return sprintf("{%s}", join(",", array_filter($regArray, "is_numeric")));
	}
}

?>
