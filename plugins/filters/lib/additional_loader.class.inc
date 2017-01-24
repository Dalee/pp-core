<?php

use PP\Plugin\PluginAbstract;

require_once dirname(__FILE__) . '/../additional/abstract.class.inc';

class PXFiltersPluginAdditionalLoader {

	private static $settingsCache = null;
	private static $classCache = array();

	/**
	 * @return array of PXAdditionalFilter
	 */
	public static function getAdditionalFilters($formatId, $pluginFolder) {
		self::fillSettingsCache();
		self::fillClassCache($formatId, $pluginFolder);
		return self::$classCache[$formatId];
	}

	private static function fillSettingsCache() {
		if (!is_null(self::$settingsCache)) {
			return;
		}
		self::$settingsCache = array();
		$additionalFilters = PluginAbstract::getParam('filters', 'additional_filters');
		if ($additionalFilters) {
			foreach (explode(',', $additionalFilters) as $f) {
				if (!$f) {
					continue;
				}
				$f = explode(':', $f);
				if (!isset($f[1])) {
					FatalError('Incorrect param additional_filters (plugins.xml)');
				}
				if (!isset(self::$settingsCache[$f[0]])) {
					self::$settingsCache[$f[0]] = array();
				}
				self::$settingsCache[$f[0]][] = $f[1];
			}
		}
	}

	private static function fillClassCache($formatId, $pluginFolder) {
		if (isset(self::$classCache[$formatId])) {
			return;
		}
		self::$classCache[$formatId] = array();
		if (isset(self::$settingsCache[$formatId])) {
			foreach (self::$settingsCache[$formatId] as $classNamePart) {
				$pathRight = '/plugins/' . $pluginFolder . '/additional/' . strtolower($classNamePart) . '.class.inc';
				foreach (array('local', 'libpp') as $place) {
					if(file_exists($fullPath = BASEPATH . $place . $pathRight)) {
						require_once $fullPath;
						$className = 'PXAdditionalFilter' . $classNamePart;
						self::$classCache[$formatId][] = new $className($formatId);
						continue(2);
					}
				}
				FatalError("Unknown filters extender - '{$classNamePart}'");
			}
		}
	}
}
