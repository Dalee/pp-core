<?php

class PXUserHTMLLang {

	protected $charset = '';
	protected $lang = '';
	protected $tree;

	public function __construct() {
		$this->setLang('rus');
	}

	public function setTree($tree) {
		$this->tree = $tree;
	}

	public function setLang($lang) {
		$this->lang = $lang;
	}

	/**
	 * smarty_modifier_lang
	 */
	public function lang_modifier($path) {
		$result = $this->_get($path);
		return false === $result ? null : $result;
	}

	/**
	 * smarty_function_lang
	 */
	public function lang($param, &$smarty) {
		if (empty($param)) {
			echo $this->lang;
			return;
		}

		$replaces = array();

		$path   = null;
		$action = null;
		$encoding = null;

		if (isset($param['get'])) {
			$path = $param['get'];
			$action = 'echo';
		}

		if (isset($param['return'])) {
			$action = 'return';
		}

		if (isset($param['date'])) {
			$date = $param['date'];
			$path = is_null($path) ? 'date' : $path;
			$action = 'date';
		}

		if (isset($param['assign'])) {
			$varname = $param['assign'];
			$action = 'assign';
		}

		if (isset($param['encoding'])) {
			$encoding = $param['encoding'];
		}

		if (is_null($path)) {
			return;
		}

		$value = $this->_get($path);
		$value = $this->_replace($value, $param);

		if (!empty($encoding)) {
			$value = $this->_reconv($value, $encoding);
		}

		switch($action) {
			case 'return':
				return $value;
				break;

			case 'assign':
				$smarty->assign($param['assign'], $value);
				break;

			case 'date':
				echo $this->_date($date, $value);
				break;

			case 'get':
			default:
				echo $value;
				break;
		}
	}

	function _get($path) {
		$path = explode('.', $path);

		$leaf = $this->tree;

		foreach ($path as $p) {
			switch (array(isset($leaf[$p]), isset($leaf[$this->lang][$p]))) {
				case array(false, true):
					$leaf = $leaf[$this->lang][$p];
					break;
				case array(false, false):
					return false;
				default:
					$leaf = $leaf[$p];
			}
		}

		if (is_string($leaf)) {
			return $leaf;
		}

		if (!isset($leaf[$this->lang])) {
			return false;
		}

		return $leaf[$this->lang];
	}

	function _date($date, $format) {
		$time = $this->parseDateToTime($date);

		$replaces = array_merge(
			$this->_dateWeekdaysReplaces($time),
			$this->_dateMonthsReplaces($time),
			$this->_dateDayReplaces($time)
		);

		$format = str_replace(array_keys($replaces), $replaces, $format);

		return strftime($format, $time);
	}

	function _dateDayReplaces($time) {
		$day = strftime('%d', $time);

		return array(
			'%e' => (
				$day > 9 ? $day : $day{1}
			)
		);
	}

	function _dateWeekdaysReplaces($time) {
		$weekDay = strftime('%u', $time);
		$short   = $this->_get('weekdays.short');
		$full    = $this->_get('weekdays'); 

		return array(
			'%a' => $short[$weekDay -1],
			'%A' => $full[$weekDay -1]
		);
	}

	function _dateMonthsReplaces($time) {
		$monthNumber = (int)strftime('%m', $time);

		$months      = $this->_get('months'); 
		$monthsShort = $this->_get('months.short'); 
		$months2     = $this->_get('months.genitive');       // Родительный падеж
		$months3     = $this->_get('months.prepositional');  // Предложный падеж

		return array(
			'%b'  => $monthsShort[$monthNumber],
			'%B3' => $months3[$monthNumber],
			'%B2' => $months2[$monthNumber],
			'%B'  => $months[$monthNumber],
			'%h'  => $monthsShort[$monthNumber]
		);
	}

	function _replace($value, $param) {
		$replaces = array();

		foreach($param as $k=>$p) {
			if(strpos($k, '_') !== 0) {
				continue;
			}

			$k{0} = '%';
			$replaces[$k.'%'] = $p;
		}

		if(sizeof($replaces)) {
			$value = str_replace(array_keys($replaces), $replaces, $value);
		}

		return $value;
	}

	function _reconv($value, $encoding) {
		if ($encoding == 'utf8') {
			$this->charset = 'u';
		}
		array_walk_recursive($value, array($this, '_myconv'));
		return $value;
	}

	function _myconv(&$v) {
		$v = myconv('k', $this->charset, $v);
	}

	function parseDateToTime($string) {
		if(strlen($string)) {
			if(strlen($string) == 10 && is_numeric($string)) {
				return $string;
			}

			preg_match("/^(\d{2})\.(\d{2})\.(\d{4})\s+(\d{2}):(\d{2}):(\d{2})(\.\d+)?$/si", trim($string), $date);
			return mktime($date[4], $date[5], $date[6], $date[2], $date[1], $date[3]);

		} else {
			return time();
		}
	}
}
