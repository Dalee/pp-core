<?php

class PXDisplayTypeMultiRegions extends PXDisplayTypeBubbleText {

	static $scriptsIncluded = false;
	private static $countInCell = 2;
	private static $chooseAll = '- ðõâìéëï÷áôø ÷ï ÷óåè òåçéïîáè -';
	private $singleObject = false;

	protected function _getValues($field, $object, $regions) {
		$cutOutRegions = isset($object['id']) && intval($object['id']) > 0 ? $regions->getCutOutRegions($field->typeDescription, $object) : null;

		$values = $regions->getRegionsAsDirectory(true, $cutOutRegions);

		#fixme it is ugly
		unset($values[null]);

		if (!$values) {
			return;
		}

		if (count($values) == 1) {
			$this->singleObject = true;
			return $values;
		}

		$values[PXMultiRegions::MAGIC] = self::$chooseAll;

		return $values;
	}

	protected function _getValue($regions, $object, $field) {
		return !empty($object[$field->name]) ? $object[$field->name] : (array) $regions->getRid();
	}

	function buildInput($field, $object, $param=NULL) {
		$regions = PXMultiRegions::getInstance();
		$values = $this->_getValues($field, $object, $regions);

		if (!is_array($values)) {
			return;
		}

		$value = $this->_getValue($regions, $object, $field);

		$html[] = new PXInputHidden($field->name . "_objectform", true);

		if (!$this->singleObject) {
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

	protected function _buildStatic($field, $value, $values) {
		return implode(", ", $values);
	}

	function getFromRequest($field, $object, $param=NULL) {
		$regionsData = max(array(), $param['request']->getVar($field->name));
		$regions = PXMultiRegions::getInstance();

		if (in_array(PXMultiRegions::MAGIC, $regionsData)) {
			$regionsData = array_keys(
				$regions->getRegionsAsDirectory(
					true,
					$regions->getCutOutRegions($field->typeDescription, $object)
				)
			);
		}

		return $regionsData;
	}

	function buildCell($field, $object, $param=NULL) {
		list($result, $isShowBubble, $allowedRegions, $inObject) = $this->collapse($field, $object);

		if ($isShowBubble) {
			$this->bubbleTextFullContent = join(", ", array_intersect_key($allowedRegions, $inObject));
			$this->bubbleTextShortContent = $result;
		}

		return $isShowBubble ? parent::buildCell($field, $object, $param) : $result;
	}

	private
	function collapse($field, $object) {
		$regions = PXMultiRegions::getInstance();

		$allowedRegions = GetColFromTableWithIndexs(
			$regions->getAllowedRegions(),
			'title'
		);

		$inObject = GetColFromTableWithIndexs(
			array_intersect_key(
				$regions->getRegionsData(),
				array_flip($object[$field->name])
			),

			"title"
		);

		$diff = array_diff_key($allowedRegions, $inObject);

		$allowedRegionsCount = count($allowedRegions);
		$inObjectCount = count($inObject);

		$isShowBubble = false;
		$result = "";

		if ($allowedRegionsCount == 1) {
			$result = reset($allowedRegions);

		} elseif (empty($diff)) {
			$result = "×ÓÅ ÒÅÇÉÏÎÙ";
			$isShowBubble = true;

		} else {
			if ($inObjectCount > self::$countInCell) {
				$result = sprintf(
					"%s + ÅÝÅ %s ÒÅÇÉÏ%s",
					join(", ", array_slice($inObject, 0, self::$countInCell)),
					$inObjectCount - self::$countInCell,
					NumericEndingsRussian($inObjectCount - self::$countInCell, "Î", "ÎÁ", "ÎÏ×")
				);

				$isShowBubble = true;

			} else {
				$result = join(", ", $inObject);
			}
		}

		if ($inObjectCount > $allowedRegionsCount) {
			$result = sprintf("<b>%s</b>", $result);
		}

		return array($result, $isShowBubble, $allowedRegions, $inObject);
	}

	function buildTreeHover($field, $object, $param=NULL) {
		list($result, $isShowBubble, $allowedRegions, $inObject) = $this->collapse($field, $object);

		return $result;
	}
}
?>
