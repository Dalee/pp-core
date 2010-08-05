<?php

class PXDisplayTypeMultiRegions extends PXDisplayTypeBubbleText {
	static $scriptsIncluded = false;
	private $singleObject = false;

	private function _getValues($field, $object, $regions) {
		$cutOutRegions = isset($object['id']) && intval($object['id']) > 0 
			? $regions->getCutOutRegions($field->typeDescription, $object)
			: null;

		$values  = $regions->getRegionsAsDirectory(true, $cutOutRegions);

		if(count($values) == 1) {
			$this->singleObject = true;
			return $values;
		}

		if(!$values) {
			return;
		}

		$values[PXMultiRegions::MAGIC] = '- ðõâìéëï÷áôø ÷ï ÷óåè òåçéïîáè -';
		
		#fixme it is ugly
		unset($values[null]);

		return $values;
	}

	private function _getValue($regions, $object, $field) {
		return !empty($object[$field->name]) ? $object[$field->name] : (array)$regions->getRid();
	}

	function buildInput($field, $object, $param=NULL) {
		$regions = PXMultiRegions::getInstance();
		$values = $this->_getValues($field, $object, $regions);

		if(!is_array($values)) {
			return;
		}

		$value  = $this->_getValue($regions, $object, $field);

		$html[] = new PXInputHidden($field->name . "_objectform", true);

		if(!$this->singleObject) {
			$html[] = $this->_buildSelector($field, $value, $values);
		} else {
			$html[] = $this->_buildStatic($field, $value, $values);
		}

		return join($html);
	}

	private function _buildSelector($field, $value, $values) {
		#delete not allowed value
		$value = array_intersect($value, array_keys($values));

		$regionSelector = new PXInputDropDown($field->name . '[]', $value, $values);
		$regionSelector->setMultiple(true);
		$regionSelector->setSize(sizeof($values));

		return $regionSelector->html();
	}

	private function _buildStatic($field, $value, $values) {
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
		$regions     = PXMultiRegions::getInstance()->loadDirectory();

		if(!$regions) {
			return $regionsData;
		}

		switch(true){
			case in_array(PXMultiRegions::MAGIC, $regionsData):
				$regionsData = array_keys($regions->getList());
				break;
		}

		return $regionsData;
	}

	function buildCell($field, $object, $param=NULL) {
		$regions = PXMultiRegions::getInstance();
		$values = $this->_getValues($field, $object, $regions);

		if(!is_array($values)) {
			return;
		}

		$value   = $this->_getValue($regions, $object, $field);
		$html    = $this->_buildStatic($field, $value, $values);

		$regionTitles = explode(",", $html);
		$regionTitlesCount = count($regionTitles);

		if($regionTitlesCount > 2) {
			$this->bubbleTextShortContent = sprintf(
				"%s + ÅÝÅ %s ÒÅÇÉÏÎÏ×",
				join(", ", array_slice($regionTitles, 0, 2)),
				$regionTitlesCount - 2
			);

			$this->bubbleTextFullContent = $html;

			return parent::buildCell($field, $object, $param);
		}

		return $html;
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
