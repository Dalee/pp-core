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

	static function switchRegion($host, $rid) {
		$regions     = PXMultiRegions::getInstance();
		$regionsData = $regions->getRegionsData();
		
		if (!isset($regionsData[$rid])) {
			return;
		}

		$engPrfx = PXRegistry::getApp()->getProperty('ENGLISH_HOST_PREFIX', 'english');

		if (strstr($host, $engPrfx)) {
			$host = substr_replace($host, $engPrfx . '.' , '');
			$isEng = true;
		}

		$currentRegion = $regionsData[$rid];
		$urlPattrens   = explode(",", $currentRegion['url']);
		$host          = explode(".", $host);

		$reversedHost = array_reverse($host);
		$result = array();

		foreach($urlPattrens as $k => $i) {
			$iArr = explode(".", $i);

			if (current($iArr) == $engPrfx) {
				unset($iArr[0]);
			}

			$h = join(".", $iArr);

			$reversedPattern = array_reverse($iArr);

			foreach($reversedPattern as $n => $k) {
				if (isset($reversedHost[$n]) && ($k == $reversedHost[$n] || $k == '*')) {
					$result[$h][] = ($k == '*') ? $reversedHost[$n] : $k;
				} elseif ($n >= count($reversedHost) - 1) {
					$result[$h][] = $k;
				}
			}

			$result[$h] = array_values(array_flip(array_flip($result[$h])));
		}

		uasort($result, array(__CLASS__, 'sortRegions'));
	}

	private static function sortRegions($a, $b) {
		$a = count($a);
		$b = count($b);

		if ($a == $b) {
			return 0;
		}

		return $a < $b ? 1 : -1;
	}
}

?>
