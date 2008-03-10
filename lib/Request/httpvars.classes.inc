<?
require_once('Common/functions.array.inc');

class NLHTTPVarsData {
	var $raw;
	var $clientCharset;
	var $_needToConvert;
	var $_needToStripTags;

	function NLHTTPVarsData($needToStripTags = false, $needToConvert=true) {
		$this->raw              = array();
		$this->clientCharset    = isset($_SERVER['CHARSET']) ? strtolower($_SERVER['CHARSET']{0}) : 'w';

		$this->_needToConvert   = $needToConvert;
		$this->_needToStripTags = $needToStripTags;
	}

	function _NormalizeData() {
		$this->clientCharset = 'w';

		foreach ($this->raw as $k=>$v) {
			if (is_string($v) && !isAsciiString($v) && isUtfString($v)) {
				$this->clientCharset = 'u';
			}
		}

		if (isset($this->raw['charcheck'])) {
			$this->_needToConvert = true;
			switch ($this->raw['charcheck']) {
				case 'éöóêåí':
				case 'ëçèìåï':
				case '??????':
					$this->clientCharset = 'w';
					break;

				case 'ÊÃÕËÅÎ':
					$this->clientCharset = 'k';
					break;

				case chr(208).chr(185).chr(209).chr(134).chr(209).chr(131).chr(208).chr(186).chr(208).chr(181).chr(208).chr(189):
					$this->clientCharset = 'u';
					break;
			}
		}

		$this->_Normalize($this->raw);
	}

	function _Normalize(&$data) {
		if (is_array($data)) {
			foreach ($data as $k=>$v) {
				$this->_Normalize($data[$k]);
			}
		} else {
			if (get_magic_quotes_gpc()) {
				$data = stripslashes($data);
			}

			if ($this->_needToStripTags) {
				$data = strip_tags($data);
			}

			if ($this->_needToConvert) {
				$data = myconv($this->clientCharset, 'k', $data);
			}
		}
	}

	function _GetVar($varName) {
		if (!isset($this->raw[$varName])) {
			return NULL;
		} else {
			return $this->raw[$varName];
		}
	}

	function _isSetVar($varName) {
		return key_exists($varName, $this->raw);
	}

	function _SetVar($varName, $value) {
		$this->raw[$varName] = $value;
	}

	function _GetNumericVar($varName) {
		if (!isset($this->raw[$varName])) {
			return NULL;
		}
		if (!is_numeric($this->raw[$varName])) {
			return NULL;
		}
		return $this->raw[$varName];
	}

	function _GetArrayVar($varName, $killEmptyRows = true) {
		if (!isset($this->raw[$varName])) {
			return array();
		}
		if (!is_array($this->raw[$varName])) {
			return array();
		}
		$temp = $this->raw[$varName];
		foreach ($temp as $k=>$v) {
			if (is_array($v)) {
				$idx = 0;
				foreach ($v as $sk=>$sv) {
					if (empty($sv) && $killEmptyRows) $idx++;
				}
				if ($idx == count($v) && $killEmptyRows) unset($temp[$k]);
			} else {
				if (empty($v) && $killEmptyRows) unset($temp[$k]);
			}
		}
		return $temp;
	}

	function _GetCheckboxVar($varName) {
		if (!isset($this->raw[$varName])) {
			return false;
		}

		return true;
	}

	function _GetTimeStampVar($varName) {
		if (!isset($this->raw[$varName])) {
			return date("d-n-Y G:i:s");
		} elseif($this->raw[$varName]['year'] == '' && $this->raw[$varName]['month'] == '' && $this->raw[$varName]['day'] == '') {
			return NULL;
		} else {
			return $this->raw[$varName]['day'].'.'.$this->raw[$varName]['month'].
				'.'.$this->raw[$varName]['year'].' '.$this->raw[$varName]['hour'].
				':'.$this->raw[$varName]['minute'].':'.$this->raw[$varName]['second'];
		}
	}

	function _GetDateVar($varName) {
		if (!isset($this->raw[$varName])) {
			return date('j-n-Y');
		} else {
			return $this->raw[$varName]['day'].'.'.$this->raw[$varName]['month'].
				'.'.$this->raw[$varName]['year'];
		}
	}

	function _GetIPAddrVar($varName) {
		if (isset($this->raw[$varName])) {
			if (is_array($this->raw[$varName])) {
				return ip2long((!empty($this->raw[$varName][0]) ? $this->raw[$varName][0] : '0')."."
					. (!empty($this->raw[$varName][0]) ? $this->raw[$varName][1] : '0')."."
					. (!empty($this->raw[$varName][0]) ? $this->raw[$varName][2] : '0')."."
					. (!empty($this->raw[$varName][0]) ? $this->raw[$varName][3] : '0'));
			} else {
				return $this->raw[$varName];
			}
		}
	}

	function _IsSet($varName) {
		return isset($this->raw[$varName]);
	}

	function GetAll() {
		$retArray = array();
		foreach ($this->raw as $k=>$v) {
			if (is_array($v)) {
				$retArray[$k] = $this->_GetArrayVar($k);
			} else {
				$retArray[$k] = $this->_GetVar($k);
			}
		}
		return $retArray;
	}

}

class NLGetData extends NLHTTPVarsData {

	function NLGetData($needToStripTags = false) {
		$this->NLHTTPVarsData($needToStripTags);
		$this->raw = $_GET;
		$this->_NormalizeData();
	}

}

class NLPostData extends NLHTTPVarsData {

	function NLPostData($needToStripTags = false) {
		$this->NLHTTPVarsData($needToStripTags);
		$this->raw = $_POST;
		$this->_NormalizeData();
	}

	function _GetImageVar($varName, $isArray=FALSE) {
		$retArray = array();

		if (!empty($_FILES[$varName]['name'])) {
			$retArray = array_merge($retArray, $_FILES[$varName]);

			$this->_Normalize($retArray);

			if ($isArray) {
				$tmpArray = $retArray['name'];
				$retArray['name'] = array();
				foreach ($tmpArray as $k=>$v) {
					$retArray['name'][$k] = stripslashes($v);
				}
			} else {
				$retArray['name'] = stripslashes($retArray['name']);
			}
		}
		if (isset($this->raw[$varName.'_remove']) && is_array($this->raw[$varName.'_remove'])) {
			$retArray = array_merge($retArray, array('remove' => array_keys($this->raw[$varName.'_remove'])));
		}
		return $retArray;
	}

	function _GetFileVar($varName, $isArray=FALSE) {
		return $this->_GetImageVar($varName, $isArray);
	}

}

class NLCookieData extends NLHTTPVarsData {

	function NLCookieData($needToStripTags = false) {
		$this->NLHTTPVarsData($needToStripTags, true);
		$this->raw = $_COOKIE;
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
		if (!isset($this->raw[$varName])) {
			return NULL;

		} else {
			$value = $this->raw[$varName];

			if(is_string($value) && strlen($value) && substr($value, 0, 2) == 'a:') {
				$value = unserialize($value);
			}

			return $value;
		}
	}
}

?>
