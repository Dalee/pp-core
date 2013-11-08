<?php
class NLCookieData extends NLHTTPVarsData {
	function __construct($needToStripTags = false) {
		parent::__construct($needToStripTags, true);
		$this->raw = (array) $_COOKIE;
		$this->_NormalizeData();
	}

	function _GetVar($varName) {
		if (!isset($this->raw[$varName])) {
			return NULL;
		}

		$value = $this->raw[$varName];

		if(is_string($value) && mb_strlen($value) && mb_substr($value, 0, 2) == 'a:') {
			$value = unserialize($value);
		}

		return $value;
	}
}
?>
