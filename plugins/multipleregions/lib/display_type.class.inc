<?php

class PXDisplayTypeMultiRegions extends PXDisplayTypeBubbleText {
	static  $scriptsIncluded     = false;

	private static $countInCell  = 2;
	private static $chooseAll    = '- ðõâìéëï÷áôø ÷ï ÷óåè òåçéïîáè -';

	private $singleObject        = false;

	private function _getValues($field, $object, $regions) {
		$cutOutRegions = isset($object['id']) && intval($object['id']) > 0 
			? $regions->getCutOutRegions($field->typeDescription, $object)
			: null;

		$values  = $regions->getRegionsAsDirectory(true, $cutOutRegions);

		if (!$values) {
			return;
		}

		if (count($values) == 1) {
			$this->singleObject = true;
			return $values;
		}

		$values[PXMultiRegions::MAGIC] = self::$chooseAll;

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

	function setDefaultValue($value) {
		return empty($value) ? PXMultiRegions::MAGIC : $value;
	}

	private function _buildSelector($field, $value, $values) {
		#delete not allowed value
		$value = array_intersect($value, array_keys($values));

		$regionSelector = new PXInputDropDown($field->name . '[]', $this->setDefaultValue($value), $values);
		$regionSelector->setMultiple(true);
		$regionSelector->setSize(sizeof($values));

		return $regionSelector->html();
	}

	private function _buildStatic($field, $value, $values) {
		$tmp = array();

		foreach($value as $v) {
			if(!isset($values[$v])) continue;
			$tmp[$v] = strtr($values[$v], array("=" => ""));
		}

		return implode(", ", empty($tmp) ? $values : $tmp);
	}

	function getFromRequest($field, $object, $param=NULL) {
		$regionsData = max(array(), $param['request']->getVar($field->name));
		$regions     = PXMultiRegions::getInstance()->loadDirectory();

		if(!$regions) {
			return $regionsData;
		}

		switch(true){
			case in_array(PXMultiRegions::MAGIC, $regionsData):
				$regionsData = array_keys($regions);
				break;
		}

		return $regionsData;
	}

	function buildCell($field, $object, $param=NULL) {
		$regions = PXMultiRegions::getInstance();
		$values = $this->_getValues($field, $object, $regions);

		static $allRegionsIds;

		if(!is_array($values)) {
			return;
		}

		if(is_null($allRegionsIds)){
			$allRegionsIds = array_keys($regions->getRegionsData());
		}

		$value      = array_intersect($allRegionsIds, $this->_getValue($regions, $object, $field));
		$html       = $this->_buildStatic($field, $value, $values);

		$regionTitles       = explode(",", $html);
		$regionTitlesCount  = count($regionTitles);
		$countAllowed       = $regions->getCountAllowedRegions();
		$moreThanOne        = $regionTitlesCount > 2;

		if ($moreThanOne) {
			$diff = array_diff($regions->getAllowedRegionsIds(), $object[PXMultiRegions::REGION_MARK]);

			if (empty($diff)) {
				$this->bubbleTextShortContent = '×ÓÅ ÒÅÇÉÏÎÙ';
			} else {
				$substractShortViewCount = $regionTitlesCount - self::$countInCell;

				$this->bubbleTextShortContent = sprintf(
					"%s + ÅÝÅ %s ÒÅÇÉÏ%s",
					join(", ", array_slice($regionTitles, 0, self::$countInCell)),
					$substractShortViewCount,
					NumericEndingsRussian($substractShortViewCount, "Î", "ÎÁ", "ÎÏ×")
				);
			}

			$this->bubbleTextFullContent  = $html;
		}

		if(count($value) > $regionTitlesCount){
			$html = $this->bubbleTextShortContent = sprintf('<b>%s</b>', $moreThanOne ? $this->bubbleTextShortContent : $html);
		}

		return $moreThanOne ? parent::buildCell($field, $object, $param) : $html;
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
