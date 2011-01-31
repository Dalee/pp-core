<?php
	final class PXDecorativeWidgetsCollection {
		private 
			static $namedData               = array();
			private static $objectsNameHash = array();
			private static $objectsTypeHash = array();
		
		private 
			function __construct(){}
		
			private static function hash_key($object){
				return strtolower(is_object($object) ? get_class($object) : $object);
			}
			
			private static function removeFromCollection($hash, $key){
				$count = 0;
				if(!empty(self::${$hash}[$key])){
					foreach(self::${$hash}[$key] as &$itemToDestroy){
						$itemToDestroy = null;
						$count++;
					}
					unset(self::${$hash}[$key]);
				}
				return $count;
			}
		
		public
			static function fill(PXAdminComplexWidget $decorableWidget){
				$key    = self::hash_key($decorableWidget);
				if(!empty(self::$namedData[$key])){
					foreach(self::$namedData[$key] as $decorableAreal => $_){
						self::fillByType($decorableWidget, $decorableAreal);
					}
				}
			}
			
			static function fillByType(PXAdminComplexWidget $decorableWidget, $decorableAreal){
				$key    = self::hash_key($decorableWidget);
				$subkey = self::hash_key($decorableAreal);
				if(!empty(self::$namedData[$key][$subkey])){
					foreach(self::$namedData[$key][$subkey] as $label => $widgets){
						foreach($widgets as $widget){
							if($widget){
								$wii = clone $widget;
								$wii->setParent($decorableWidget);
								$decorableWidget->append($wii, $label);
							}
						}
					}
				}
			}

			private static function initVars(PXAdminWidget $widget, $classToDecorate, $decorableAreal, $decorablePart, $alias = 'default') {
				return array(
					"wname"  => self::hash_key($widget),
					"key"    => self::hash_key($classToDecorate),
					"subkey" => self::hash_key($decorableAreal),
					"alias"  => self::hash_key($alias));
			}

			private static function putInLists(PXAdminWidget $widget, $key, $wname, $subkey, $alias) {
				self::$objectsNameHash[$key . $wname][]           = $widget;
				self::$objectsTypeHash[$subkey . $alias][]        = $widget;
			}

			static function addToCollection(PXAdminWidget $widget, $classToDecorate, $decorableAreal, $decorablePart, $alias = 'default'){
				extract(self::initVars($widget, $classToDecorate, $decorableAreal, $decorablePart, $alias));

				self::$namedData[$key][$subkey][$decorablePart][] = $widget;
				self::putInLists($widget, $key, $wname, $subkey, $alias);
			}

			static function addToCollectionBefore($beforeAreal, PXAdminWidget $widget, $classToDecorate, $decorableAreal, $decorablePart, $alias = 'default') {
				extract(self::initVars($widget, $classToDecorate, $decorableAreal, $decorablePart, $alias));

				self::addToPosition(0, $beforeAreal, $widget, $classToDecorate, $decorableAreal, $decorablePart, $alias);
				self::putInLists($widget, $key, $wname, $subkey, $alias);
			}

			static function addToCollectionAfter($afterAreal, PXAdminWidget $widget, $classToDecorate, $decorableAreal, $decorablePart, $alias = 'default') {
				extract(self::initVars($widget, $classToDecorate, $decorableAreal, $decorablePart, $alias));

				self::addToPosition(1, $afterAreal, $widget, $classToDecorate, $decorableAreal, $decorablePart, $alias);
				self::putInLists($widget, $key, $wname, $subkey, $alias);
			}

			private static function addToPosition($shift, $desiredAreal, PXAdminWidget $widget, $classToDecorate, $decorableAreal, $decorablePart, $alias = 'default') {
				extract(self::initVars($widget, $classToDecorate, $decorableAreal, $decorablePart, $alias));

				$desiredAreal = strtolower($desiredAreal);
				$merged = array($subkey => array($decorablePart => array($widget)));

				if (!isset(self::$namedData[$key])) {
					self::$namedData[$key] =  $merged;
					return;
				}

				$arealToPosition = array_flip(array_keys(self::$namedData[$key]));
				$position = !isset($arealToPosition[$desiredAreal]) ? -1 : $arealToPosition[$desiredAreal];

				if ($position < 0) {
					if ($shift == 1) {
						self::$namedData[$key] = array_merge(self::$namedData[$key], $merged);
					} elseif($shift == 0) {
						self::$namedData[$key] = array_merge($merged, self::$namedData[$key]);
					}
				} else {
					$before = array_slice(self::$namedData[$key], 0, $position  + $shift);
					$after  = array_slice(self::$namedData[$key], $position);

					self::$namedData[$key] = array_merge($before, $merged, $after);
				}
			}

			static function removeFromCollectionByName($classToDecorate, $className){
				return self::removeFromCollection('objectsNameHash', self::hash_key($classToDecorate) . self::hash_key($className));
			}
		
			static function removeFromCollectionByType($decorableAreal, $alias = 'default'){
				return self::removeFromCollection('objectsTypeHash', self::hash_key($decorableAreal) . self::hash_key($alias));
			}
	}
?>
