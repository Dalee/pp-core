<?php
class PXAbstractCronRun {
	public $name = 'Abstract CronRun Class';

	function __construct() {}

	function Run(&$app, &$db, &$tree, $matchedTime, $matchedRule) {
		return array(
			'status' => -1,
			'note'   => 'Не определен метод Run()'
		);
	}

	function log($message) {
		$logger =& $this->getLogger();
		$logger->log($logger->INFO, $message);
	}

	function error($message) {
		$logger =& $this->getLogger();
		$logger->log($logger->ERROR, $message);
	}

	function &getLogger() {
		if(!isset($this->logger)) {
			$logsDir = BASEPATH.'var/logs/cronrun/'.str_replace('pxcronrun', '', get_class($this)).'/';
			MakeDirIfNotExists($logsDir);

			$this->logger =& NLLogger::getLogger('cron');

			if(PXRequest::getHttpMethod() === 'CLI') {
				$this->logger->addAppender('file', 'FileAppender');
			} else {
				$this->logger->addAppender('file', 'Appender');
			}

			$this->logger->setLevel($this->logger->INFO);
			$this->logger->setAppenderParam('file', 'fileName', $logsDir.date('Y-m-d_H-i-s').'.log');
		}

		return $this->logger;
	}
}

class PXCronRule {
	var $valid;
	var $asString;
	var $match;
	var $matchHash;

	function PXCronRule($ruleString) {
		$this->valid = false;
		$this->asString = $ruleString;

		$params = split(' +', $ruleString);

		if (count($params) != 5) {
			return;
		}

		$this->match['min']  = $this->_parse($params[0], 0, 59);
		$this->match['hour'] = $this->_parse($params[1], 0, 23);
		$this->match['mday'] = $this->_parse($params[2], 1, 31);
		$this->match['mon']  = $this->_parse($params[3], 1, 12);
		$this->match['wday'] = $this->_parse($params[4], 0, 7);

		if (in_array(NULL, $this->match)) {
			return;
		}

		if (isset($this->match['wday'][7])) {
			unset($this->match['wday'][7]);
			$this->match['wday'][0] = true;
		}

		$this->matchHash = md5(serialize($this->match));

		$this->valid = true;
	}

	function _parse($s, $min, $max) {
		$result = array();
		$s = strtolower($s);

		$s = strtr($s,
			array(
				'sun' => '0',
				'mon' => '1',
				'tue' => '2',
				'wed' => '3',
				'thu' => '4',
				'fri' => '5',
				'sat' => '6'
			)
		);

		$s = strtr($s,
			array(
				'jan' =>  '1',
				'feb' =>  '2',
				'mar' =>  '3',
				'apr' =>  '4',
				'may' =>  '5',
				'jun' =>  '6',
				'jul' =>  '7',
				'aug' =>  '8',
				'sep' =>  '9', 
				'oct' => '10',
				'nov' => '11',
				'dec' => '12'
			)
		);

		$params = explode(',', $s);
		foreach ($params as $k) {
			$step = 1;

			if (preg_match('#^(.+?)/(\d+)$#', $k, $m)) {
				$k    = $m[1];
				$step = (int) $m[2];

				if ($step <= 0 || $step >= $max) {
					return NULL;
				}
			}

			if (preg_match('#^(\d+)-(\d+)$#', $k, $m)) {
				if ($m[1] >= $m[2]) {
					return NULL;
				}

				if ($m[1] < $min || $m[2] > $max) {
					return NULL;
				}

				for($i = $m[1]; $i <= $m[2]; $i += $step) {
					$result[$i] = true;
				}

			} else if (preg_match('#^(\d+|\*)$#', $k, $m)) {
				if ($m[1] == '*') {
					for ($i = $min; $i <= $max; $i += $step) {
						$result[$i] = true;
					}

				} else {
					if ($m[1] < $min || $m[1] > $max) {
						return NULL;
					}

					$result[$m[1]] = true;
				}

			} else {
				return NULL;
			}
		}

		return $result;
	}
}
?>
