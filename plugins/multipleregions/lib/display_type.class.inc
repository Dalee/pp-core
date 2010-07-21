<?php

class PXDisplayTypeMultiRegions extends PXDisplayType {
	const MAGIC = -1;

	private function _getValues($regions) {
		$values  = $regions->getRegionsAsDirectory();

		if(!$values){
			return;
		}

		$values[self::MAGIC] = '- ðõâìéëï÷áôø ÷ï ÷óåè òåçéïîáè -';
		$values[null]        = '- òåçéïî îå îáúîáþåî -';

		return $values;
	}
	private function _getValue($regions, $object, $field) {
		return !empty($object[$field->name]) ? $object[$field->name] : (array)$regions->getRid();
	}


	function buildInput($field, $object, $param=NULL) {
		$regions = PXMultiRegions::getInstance();
		$values = $this->_getValues($regions);

		if(!is_array($values)) {
			return;
		}

		$value   = $this->_getValue($regions, $object, $field);

		if(PXRegistry::getUser()->can('manage_region')){
			return $this->_buildSelector($field, $value, $values);
		} else {
			return $this->_buildStatic($field, $value, $values);
		}
	}

	private function _buildSelector($field, $value, $values) {
		$regionSelector = new PXInputDropDown($field->name . '[]', $value, $values);
		$regionSelector->setMultiple(true);
		$regionSelector->setSize(sizeof($values));

		return $regionSelector->html();
	}

	private function _buildStatic($field, $value, $values) {
		if(in_array(self::MAGIC, $value)) {
			return $values[self::MAGIC];
		}

		foreach($value as $k=>$v) {
			if(isset($values[$v])) {
				$value[$k] = preg_replace('/^=+/', '', $values[$v]);
			} else {
				unset($value[$k]);
			}
		}

		return implode(", ", $value);
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

	function buildCell($field, $object, $param=NULL) {
		$regions = PXMultiRegions::getInstance();
		$values = $this->_getValues($regions);

		if(!is_array($values)) {
			return;
		}

		$value   = $this->_getValue($regions, $object, $field);
		return  $this->_buildStatic($field, $value, $values);
	}
}

/**
 * ôÁÂÌÉÃÁ URL ÒÅÇÉÏÎÏ×
 *
 * storageType regionurl
 * displayType URLTABLE
 */

class PXDisplayTypeUrltable extends PXDisplayTypeTable {
	function buildInput($field, $object, $param=NULL) {
		$value  = isset($object[$field->name]) ? explode(',', trim($object[$field->name])) : array();
		foreach ($value as $k => $v) {
			$value[$k] = (array)$v;
		}

		$height = 0;
		$cols   = '1';
		$header = array();

		return NLAbstractHTMLForm::BuildTableByCols($field->name, $value, $cols, $height, $header);
	}
}
	
?>
