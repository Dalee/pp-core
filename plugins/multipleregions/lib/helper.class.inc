<?php

class PXMultipleRegionsHelper {
	static function toArray($regString) {
		return self::uniq(array_filter(explode(",", strtr($regString, array("{" => "", "}" => ""))), "is_numeric"));
	}

	static function toString($regArray) {
		return sprintf("{%s}", join(",", array_filter(self::uniq($regArray), "is_numeric")));
	}

	static function uniq($regArray) {
		return array_flip(array_flip($regArray));
	}
}

?>
