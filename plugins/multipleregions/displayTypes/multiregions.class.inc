<?php

class PXDisplayTypeMultiRegions extends PXDisplayTypeBubbleText {

	static $scriptsIncluded = false;
	private static $countInCell = 2;
	private static $chooseAll = '- ПУБЛИКОВАТЬ ВО ВСЕХ РЕГИОНАХ -';
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

	protected function _getSubregionsForMacroregions($regions) {
		$ids = array();
		foreach ($regions->getRegionsData() as $region) {
			if (empty($region['parent'])) {
				continue;
			}
			if (empty($ids[$region['parent']])) {
				$ids[$region['parent']] = array();
			}
			array_push($ids[$region['parent']], $region['id']);
		}
		foreach ($ids as $k => $id) {
			$ids[$k] = join(',', $id);
		}
		return $ids;
	}

	function buildInput($field, $object, $param=NULL) {
		$regions = PXMultiRegions::getInstance();
		$values = $this->_getValues($field, $object, $regions);

		if (!is_array($values)) {
			return;
		}

		$value = $this->_getValue($regions, $object, $field);

		if (!$this->singleObject) {
			$html[] = '<div class="help-box">';
			$html[] = new PXInputHidden($field->name . "_objectform", true);

			PXRegistry::getLayout()->assignInlineJS($this->selectorInlineJS());
			PXRegistry::getLayout()->assignInlineCSS($this->selectorInlineCSS());

			$html[] = '<div class="help-for">';
			$html[] = $this->_buildSelector($field, $value, $values);
			$html[] = '</div>';

			$html[] = '<div class="help-text">';
			$html[] = $this->helpText();
			$html[] = '</div>';

			$html[] = '</div>';
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
				@array_flip($object[$field->name])
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
			$result = "все регионы";
			$isShowBubble = true;

		} else {
			if ($inObjectCount > self::$countInCell) {
				$result = sprintf(
					"%s + еще %s регио%s",
					join(", ", array_slice($inObject, 0, self::$countInCell)),
					$inObjectCount - self::$countInCell,
					NumericEndingsRussian($inObjectCount - self::$countInCell, "н", "на", "нов")
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

	protected function selectorInlineJS() {
		$regions = PXMultiRegions::getInstance();
		$subregions = $this->_getSubregionsForMacroregions($regions);
		$js = "var multiregions_subregions = ".json_encode($subregions).";";
		$js .= <<<JS
$(function(){
	var subregions = window.multiregions_subregions||{};
	$('select[name="sys_regions[]"]').unbind('click').click(function(e) {
		var select = $(this), option = $(e.target);
		if (!option || !(/option/i).test(option[0].tagName||'') || e.altKey || e.shiftKey) {
			return;
		}
		if (select.val().indexOf('-1') != -1 || option.val() == -1) {
			var allIds = [];
			$.each(select.find('option'), function(i,el){ allIds.push($(el).val()); });
			select.val(allIds);
			return;
		}
		if (!subregions[option.val()]) {
			return;
		}

		if (select.val().indexOf(option.val()) == -1) {
			var ids = [], inner = subregions[option.val()].split(',');
			$.each(select.val(), function(i, el){
				if (inner.indexOf(el) != -1) return;
				ids.push(el);
			});
			select.val(ids);
		} else {
			select.val((subregions[option.val()] + ',' + select.val().join(',')).split(','));
		}
	});
});
JS;
		return $js;
	}

	protected function selectorInlineCSS() {
		$r = <<<CSS
div.help-box {}
div.help-box .help-for {
	float:left;
}
div.help-box .help-text {
	float:left;
	margin-left:10px;
	line-height:1.5em;
	max-width:300px;
}
CSS;
		return $r;
	}

	protected function helpText() {
		$r = <<<help
<p>По умолчанию при выборе макрорегиона автоматически выбираются его субрегионы.</p>
<p>Но при зажатой клавише Alt автовыбор не активируется и выделяется только сам макрорегион без субрегионов.</p>
<p>Так же, используйте зажатую клавижу Ctrl для последовательного выбора регионов, и Shift для выбора нескольких регионов подряд.</p>
help;
		return trim($r);
	}
}
?>
