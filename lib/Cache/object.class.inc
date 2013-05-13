<?php
class ObjectCache extends PXCacheFile { //FIXME ! Remove 'extends' ! It is only for back compatibility !

	// Factory
	// Cache definition can contains extra args string for concrete engine after its name@
	// Now cache definition is URL scheme base, with backward compatibility
	public static function get($cache, $cacheDomain = null, $defaultExpire = 3600) {
		$extraArgs = null;

		switch (true) {
			case $cache === "1":
				$cache = 'file';
				break;

			case empty($cache):
				$cache = 'null';
				break;

			case strpos($cache, '://') !== false:
				$extraArgs = parse_url($cache);
				$cache = $extraArgs['scheme'];
				break;

			case strpos($cache, '@') !== false:
				list($cache, $extraArgs) = explode('@', $cache, 2);
				break;

		}

		$klass = 'PXCache' . ucfirst($cache);
		if(!class_exists($klass)) FatalError("Caching method '{$cache}' not implemented !");
		$instance = new $klass($cacheDomain, $defaultExpire, $extraArgs);
		if(!is_a($instance, 'IPXCache')) FatalError("Caching method '{$cache}' doesn't implements IPXCache interface !");
		return $instance;
	}
}

class NullCache extends PXCacheNull {} //FIXME ! See above
?>
