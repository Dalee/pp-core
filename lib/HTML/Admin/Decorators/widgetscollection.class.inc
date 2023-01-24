<?php

final class PXDecorativeWidgetsCollection
{

	private static array $namedData = [];
	private static array $objectsNameHash = [];
	private static array $objectsTypeHash = [];

	private function __construct()
	{
	}

	private static function hash_key($object)
	{
		return strtolower(is_object($object) ? $object::class : $object);
	}

	private static function removeFromCollection($hash, $key)
	{
		$count = 0;
		if (!empty(self::${$hash}[$key])) {
			foreach (self::${$hash}[$key] as &$itemToDestroy) {
				$itemToDestroy = null;
				$count++;
			}
			unset(self::${$hash}[$key]);
		}

		return $count;
	}

	public static function fill(PXAdminComplexWidget $decorableWidget)
	{
		$key = self::hash_key($decorableWidget);
		if (!empty(self::$namedData[$key])) {
			foreach (self::$namedData[$key] as $decorableAreal => $_) {
				self::fillByType($decorableWidget, $decorableAreal);
			}
		}
	}

	public static function fillByType(PXAdminComplexWidget $decorableWidget, $decorableAreal)
	{
		$key = self::hash_key($decorableWidget);
		$subkey = self::hash_key($decorableAreal);
		if (empty(self::$namedData[$key][$subkey])) {
			return;
		}
		// for each areal
		foreach (self::$namedData[$key][$subkey] as $label => $widgets) {
			// and each !empty widget
			foreach ($widgets as $widget) {
				if (!$widget) {
					continue;
				}
				// put clone of it to parent (enclose widget to parent)
				if (!is_string($widget)) {
					$wii = clone $widget;
					$wii->setParent($decorableWidget);
				} else {
					$wii = $widget;
				}
				$decorableWidget->append($wii, $label);
			}
		}
	}

	private static function initVars($widget, $classToDecorate, $decorableAreal, $decorablePart, $alias = 'default')
	{
		return [
			"wname" => self::hash_key($widget),
			"key" => self::hash_key($classToDecorate),
			"subkey" => self::hash_key($decorableAreal),
			"alias" => self::hash_key($alias)
		];
	}

	private static function putInLists(&$widget, $key, $wname, $subkey, $alias)
	{
		self::$objectsNameHash[$key . $wname][] = &$widget;
		self::$objectsTypeHash[$subkey . $alias][] = &$widget;
	}

	public static function addToCollection($widget, $classToDecorate, $decorableAreal, $decorablePart, $alias = 'default')
	{
		$key = $subkey = $wname = null;
		$initVars = self::initVars($widget, $classToDecorate, $decorableAreal, $decorablePart, $alias);
		extract($initVars);

		self::$namedData[$key][$subkey][$decorablePart][] = &$widget;
		self::putInLists($widget, $key, $wname, $subkey, $alias);
	}

	public static function addToCollectionBefore($beforeAreal, $widget, $classToDecorate, $decorableAreal, $decorablePart, $alias = 'default')
	{
		$key = $subkey = $wname = null;
		$initVars = self::initVars($widget, $classToDecorate, $decorableAreal, $decorablePart, $alias);
		extract($initVars);

		self::addToPosition(0, $beforeAreal, $widget, $classToDecorate, $decorableAreal, $decorablePart, $alias);
		self::putInLists($widget, $key, $wname, $subkey, $alias);
	}

	public static function addToCollectionAfter($afterAreal, $widget, $classToDecorate, $decorableAreal, $decorablePart, $alias = 'default')
	{
		$key = $subkey = $wname = null;
		$initVars = self::initVars($widget, $classToDecorate, $decorableAreal, $decorablePart, $alias);
		extract($initVars);

		self::addToPosition(1, $afterAreal, $widget, $classToDecorate, $decorableAreal, $decorablePart, $alias);
		self::putInLists($widget, $key, $wname, $subkey, $alias);
	}

	private static function addToPosition($shift, $desiredAreal, &$widget, $classToDecorate, $decorableAreal, $decorablePart, $alias = 'default')
	{
		$key = $subkey = $wname = null;
		$initVars = self::initVars($widget, $classToDecorate, $decorableAreal, $decorablePart, $alias);
		extract($initVars);

		$desiredAreal = strtolower((string) $desiredAreal);
		$merged = [$subkey => [$decorablePart => [&$widget]]];

		if (!isset(self::$namedData[$key])) {
			self::$namedData[$key] = $merged;
			return;
		}

		$arealToPosition = array_flip(array_keys(self::$namedData[$key]));
		$position = !isset($arealToPosition[$desiredAreal]) ? -1 : $arealToPosition[$desiredAreal];

		if ($position < 0) {
			if ($shift == 1) {
				self::$namedData[$key] = array_merge(self::$namedData[$key], $merged);
			} elseif ($shift == 0) {
				self::$namedData[$key] = array_merge($merged, self::$namedData[$key]);
			}
		} else {
			$before = array_slice(self::$namedData[$key], 0, $position + $shift);
			$after = array_slice(self::$namedData[$key], $position);

			self::$namedData[$key] = array_merge($before, $merged, $after);
		}
	}

	public static function removeFromCollectionByName($classToDecorate, $className)
	{
		return self::removeFromCollection('objectsNameHash', self::hash_key($classToDecorate) . self::hash_key($className));
	}

	public static function removeFromCollectionByType($decorableAreal, $alias = 'default')
	{
		return self::removeFromCollection('objectsTypeHash', self::hash_key($decorableAreal) . self::hash_key($alias));
	}
}
