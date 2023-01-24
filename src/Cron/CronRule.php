<?php

namespace PP\Cron;

/**
 * Class CronRule
 * @package PP\Cron
 */
class CronRule
{

	public $valid;
	public $match;
	public $matchHash;

	public function __construct(public $asString)
	{
		$this->valid = false;

		$params = preg_split('/\s+/' . REGEX_MOD, (string) $asString);

		if ((is_countable($params) ? count($params) : 0) != 5) {
			return;
		}

		$this->match['min'] = $this->_parse($params[0], 0, 59);
		$this->match['hour'] = $this->_parse($params[1], 0, 23);
		$this->match['mday'] = $this->_parse($params[2], 1, 31);
		$this->match['mon'] = $this->_parse($params[3], 1, 12);
		$this->match['wday'] = $this->_parse($params[4], 0, 7);

		if (in_array(null, $this->match)) {
			return;
		}

		if (isset($this->match['wday'][7])) {
			unset($this->match['wday'][7]);
			$this->match['wday'][0] = true;
		}

		$this->matchHash = md5(serialize($this->match));

		$this->valid = true;
	}

	public function _parse($s, $min, $max)
	{
		$result = [];
		$s = strtolower((string) $s);

		$s = strtr($s,
			[
				'sun' => '0',
				'mon' => '1',
				'tue' => '2',
				'wed' => '3',
				'thu' => '4',
				'fri' => '5',
				'sat' => '6',
			]
		);

		$s = strtr($s,
			[
				'jan' => '1',
				'feb' => '2',
				'mar' => '3',
				'apr' => '4',
				'may' => '5',
				'jun' => '6',
				'jul' => '7',
				'aug' => '8',
				'sep' => '9',
				'oct' => '10',
				'nov' => '11',
				'dec' => '12',
			]
		);

		$params = explode(',', $s);
		foreach ($params as $k) {
			$step = 1;

			if (preg_match('#^(.+?)/(\d+)$#' . REGEX_MOD, $k, $m)) {
				$k = $m[1];
				$step = (int)$m[2];

				if ($step <= 0 || $step >= $max) {
					return null;
				}
			}

			if (preg_match('#^(\d+)-(\d+)$#' . REGEX_MOD, $k, $m)) {
				if ($m[1] >= $m[2]) {
					return null;
				}

				if ($m[1] < $min || $m[2] > $max) {
					return null;
				}

				for ($i = $m[1]; $i <= $m[2]; $i += $step) {
					$result[$i] = true;
				}

			} else if (preg_match('#^(\d+|\*)$#' . REGEX_MOD, $k, $m)) {
				if ($m[1] == '*') {
					for ($i = $min; $i <= $max; $i += $step) {
						$result[$i] = true;
					}

				} else {
					if ($m[1] < $min || $m[1] > $max) {
						return null;
					}

					$result[$m[1]] = true;
				}

			} else {
				return null;
			}
		}

		return $result;
	}
}
