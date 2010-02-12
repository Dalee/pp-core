<?php
class NLCookieData extends NLHTTPVarsData {

	function __construct($needToStripTags = false) {
		parent::__construct($needToStripTags, true);
		$this->raw = (array) $_COOKIE;
		$this->_NormalizeData();
	}

	function _SetVar($varName, $value='', $expire=NULL) {
		if(is_array($value) || is_object($value)) {
			$value = serialize($value);
		}

		$value = convert_cyr_string($value, 'k', 'w');

		if(is_numeric($expire)) {
			return setcookie($varName, $value, $expire);
		} else {
			return setcookie($varName, $value);
		}
	}

	function _GetVar($varName) {
		if (!isset($this->raw[$varName])) 
			return NULL;

		$value = $this->raw[$varName];

		if(is_string($value) && strlen($value) && substr($value, 0, 2) == 'a:') {
			$value = unserialize($value);
		}

		return $value;
	}
}

?>
