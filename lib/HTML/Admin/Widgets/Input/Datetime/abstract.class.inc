<?php
// Abstract
class PXInputDateTime extends PXAdminWidget { 
	var $name;
	var $date;   // array(year, month, day, hour, minute, second);

	function PXInputDateTime($name, $value, $notNull=true) {
		$this->name    = $name;
		$this->notNull = $notNull;

		$this->date = $this->_parseDate($value, $notNull);
	}

	function _parseDate($value, $notNull) {
		$date = array(
			'year'   => null,
			'month'  => null,
			'day'    => null,

			'hour'   => null,
			'minute' => null,
			'second' => null
		);

		if (!is_null($value)) {
			$matches = array();

			$value = trim($value);
			$value = mb_substr($value, 0, 19);

			preg_match("/^((\d{1,2}).(\d{1,2}).(\d{4}))?\s?((\d{1,2})(:(\d{1,2})(:(\d{1,2}))?)?)?/", trim($value), $matches);

			if(isset($matches[1]) && mb_strlen($matches[1])) {
				$date['day'   ] = $matches[2];
				$date['month' ] = $matches[3];
				$date['year'  ] = $matches[4];

			} else {
				$date['year'  ] = date('Y');
				$date['month' ] = date('n');
				$date['day'   ] = date('j');
			}

			if(isset($matches[5])) {
				$date['hour'  ] = $matches[6];
				$date['minute'] = $matches[8];
				$date['second'] = $matches[10];

			} else {
				$date['hour'  ] = 0;
				$date['minute'] = 0;
				$date['second'] = 0;
			}

		} elseif($notNull) {
			$date['year'   ] = date('Y');
			$date['month'  ] = date('n');
			$date['day'    ] = date('j');

			$date['hour'   ] = date('G');
			$date['minute' ] = date('i') * 1;
			$date['second' ] = date('s') * 1;
		}

		return $date;
	}

	function html() {
		FatalError("It's abstract class, use other date or time inputs class");
	}
}

?>
