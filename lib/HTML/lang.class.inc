<?php

class PXUserHTMLLang {
	function PXUserHTMLLang() {
		$this->setLang('rus');
	}

	function setTree($tree) {
		$this->tree = $tree;
	}

	function setLang($lang) {
		$this->lang = $lang;
	}

	function lang($param, &$smarty) {
		$replaces = array();

		$lang = $this->lang;

		$path   = null;
		$action = null;

		if(isset($param['get'])) {
			$path = $param['get'];
			$action = 'echo';
		}

		if(isset($param['return'])) {
			$action = 'return';
		}

		if(isset($param['date'])) {
			$date = $param['date'];
			$path = 'date';
			$action = 'date';
		}

		if(isset($param['assign'])) {
			$varname = $param['assign'];
			$action = 'assign';
		}

		foreach($param as $k=>$p) {
			if(strpos($k, '_') !== 0) {
				continue;
			}

			$k{0} = '%';
			$replaces[$k.'%'] = $p;

			$action = 'replace';
		}

		if(is_null($path)) {
			return;
		}

		$path = explode('.', $path);

		$tmp = $this->tree;
		foreach($path as $p) {
			if(!isset($tmp[$p])) {
				return false;
			}

			$tmp = $tmp[$p];
		}

		if(!isset($tmp[$lang])) {
			return false;
		}

		$value = $tmp[$lang];

		switch($action) {
			case 'return':
				return $value;
				break;

			case 'assign':
				$smarty->assign($param['assign'], $value);
				break;

			case 'date':
				$time = $this->parseDateToTime($date);
				echo smarty_modifier_date_format($time, $value);
				break;

			case 'replace':
				foreach($replaces as $m=>$r) {
					$value = str_replace($m, $r, $value);
				}

				echo $value;
				break;

			case 'get':
			default:
				echo $value;
				break;
		}
	}

	function parseDateToTime($string) {
		if(strlen($string)) {
			preg_match("/^(\d{2})\.(\d{2})\.(\d{4})\s+(\d{2}):(\d{2}):(\d{2})$/si", trim($string), $date);
			return mktime($date[4], $date[5], $date[6], $date[2], $date[1], $date[3]);

		} else {
			return time();
		}
	}
}

?>
