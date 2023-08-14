<?php

require_once PPLIBPATH . 'Common/functions.array.inc';

class NLHTTPVarsData
{
	public $raw;
	public $clientCharset;

	public function __construct(public $_needToStripTags = false, $needToConvert = true)
	{
		$this->raw = [];
		$this->clientCharset = isset($_SERVER['CHARSET']) ? strtolower((string) $_SERVER['CHARSET'][0]) : CHARSET_UTF8;
	}

	// Раскодирование JavaScript escape();
	// private
	public function _unEscape(&$data)
	{
		if (is_array($data)) {
			foreach ($data as $k => $v) {
				$this->_unEscape($data[$k]);
			}

		} else {
			if (preg_match('/^(%u\d{4})+$/' . REGEX_MOD, (string) $data)) {
				$data = utf8_urldecode($data);
			}
		}
	}

	public function _NormalizeData()
	{
		$this->_unEscape($this->raw);
		$this->_Normalize($this->raw);
	}

	public function _Normalize(&$data)
	{
		if (is_array($data)) {
			foreach ($data as $k => $v) {
				$this->_Normalize($data[$k]);
			}

		} else {
			if ($this->_needToStripTags) {
				$data = strip_tags((string) $data);
			}
		}
	}

	public function _GetVar($varName)
	{
		return $this->raw[$varName] ?? NULL;
	}

	public function _isSetVar($varName)
	{
		return array_key_exists($varName, $this->raw);
	}

	public function _SetVar($varName, $value)
	{
		$this->raw[$varName] = $value;
	}

	public function _GetNumericVar($varName)
	{
		if (!isset($this->raw[$varName])) {
			return NULL;
		}
		if (!is_numeric($this->raw[$varName])) {
			return NULL;
		}
		return $this->raw[$varName];
	}

	public function _GetArrayVar($varName, $killEmptyRows = true)
	{
		if (!isset($this->raw[$varName])) {
			return [];
		}
		if (!is_array($this->raw[$varName])) {
			return [];
		}

		$temp = $this->raw[$varName];

		static $callback;

		if (!isset($callback)) {
			$callback = function (&$arrayItself, $key, $opts) {
				if (empty($arrayItself[$key]) && !in_array($arrayItself[$key], [0, "0"], true) && $opts['killEmptyRows']) {
					unset($arrayItself[$key]);
				}
			};
		}

		nestedArrayWalk($temp, $callback, compact('killEmptyRows'));

		return $temp;
	}

	public function _GetCheckboxVar($varName)
	{
		if (!isset($this->raw[$varName])) {
			return false;
		}

		return true;
	}

	public function _GetTimeStampVar($varName)
	{
		if (!isset($this->raw[$varName])) {
			return date("d-n-Y G:i:s");
		} elseif ($this->raw[$varName]['year'] == '' && $this->raw[$varName]['month'] == '' && $this->raw[$varName]['day'] == '') {
			return NULL;
		} else {
			$r = $this->raw[$varName];

			$pattern = ["year", "month", "day", "hour", "minute", "second"];
			$diff = array_diff($pattern, array_keys($r));

			if (!empty($diff))
				return null;

			return sprintf("%02d.%02d.%04d %02d:%02d:%02d", $r['day'], $r['month'], $r['year'], $r['hour'],
				$r['minute'], $r['second']);
		}
	}

	public function _GetDateVar($varName)
	{
		if (!isset($this->raw[$varName])) {
			return date('j-n-Y');
		} else {
			$r = $this->raw[$varName];

			$reverse = array_flip($r);
			if (isset($reverse['']))
				return null;

			$pattern = ["year", "month", "day"];
			$diff = array_diff($pattern, array_keys($r));

			if (!empty($diff))
				return null;

			return call_user_func_array("sprintf", ["pattern" => "%02d.%02d.%04d",
				$r['day'], $r['month'], $r['year']]);
		}
	}

	public function _GetIPAddrVar($varName)
	{
		if (isset($this->raw[$varName])) {
			$val = implode(".", $this->raw[$varName]);
			return filter_var($val, FILTER_VALIDATE_IP) ? ip2long($val) : 0;
		}
	}

	public function _IsSet($varName)
	{
		return isset($this->raw[$varName]);
	}

	public function GetAll()
	{
		$retArray = [];
		foreach ($this->raw as $k => $v) {
			if (is_array($v)) {
				$retArray[$k] = $this->_GetArrayVar($k);
			} else {
				$retArray[$k] = $this->_GetVar($k);
			}
		}
		return $retArray;
	}

}
