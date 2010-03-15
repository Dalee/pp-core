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
		
			static function addToCollection(PXAdminWidget $widget, $classToDecorate, $decorableAreal, $decorablePart, $alias = 'default'){
				$wname  = self::hash_key($widget);
				$key    = self::hash_key($classToDecorate);
				$subkey = self::hash_key($decorableAreal);
				$alias  = self::hash_key($alias);
				
				self::$namedData[$key][$subkey][$decorablePart][] = &$widget;
				self::$objectsNameHash[$key . $wname][]           = &$widget;
				self::$objectsTypeHash[$subkey . $alias][]        = &$widget;
			}
		
			static function removeFromCollectionByName($classToDecorate, $className){
				return self::removeFromCollection('objectsNameHash', self::hash_key($classToDecorate) . self::hash_key($className));
			}
		
			static function removeFromCollectionByType($decorableAreal, $alias = 'default'){
				return self::removeFromCollection('objectsTypeHash', self::hash_key($decorableAreal) . self::hash_key($alias));
			}
	}
?>