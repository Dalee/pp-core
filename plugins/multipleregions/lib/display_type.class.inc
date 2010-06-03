<?php
	class PXDisplayTypeMultiRegions extends PXDisplayType {
		const MAGIC = -1;

		function buildInput($field, $object, $param=NULL) {
			$regions = PXMultiRegions::getInstance();
			$value   = !empty($object[$field->name]) ? $object[$field->name] : (array)$regions->getRid();
			$values  = $regions->getRegionsAsDirectory();
			if(!$values){
				return;
			}
			
			$values[self::MAGIC] = '- נץגליכןקבפר קן קףוט עוחיןמבט -';
			$values[null]        = '- עוחיןמ מו מבתמב‏ומ -';
			
			$regionSelector = new PXInputDropDown($field->name . '[]', $value, $values);
			$regionSelector->setMultiple(true);
			$regionSelector->setSize(sizeof($values));
			
			return $regionSelector->html();
		}
		
		function buildRow($field, $object, $param=NULL){
			$user = PXRegistry::getUser();
			if(!$user->can('manage_region')){
				return false;
			}
			return parent::buildRow($field, $object, $param);
		}
		
		function getFromRequest($field, $object, $param=NULL) {
			$regionsData = max(array(), $param['request']->getVar($field->name));
			$regions     = PXMultiRegions::getInstance()->getRegionsAsDirectory(false);
			if(!$regions){
				return $regionsData;
			}
			switch(true){
				case in_array(self::MAGIC, $regionsData):
					$regionsData = array_merge(array_keys($regions->getList()), array(self::MAGIC));
					break;
				case $regions->parentField && sizeof($regionsData):
					$regionsData = $regions->getTree()->GetDescendantsOrSelf($regionsData);
			}
			return $regionsData;
		}
	}
	
?>
