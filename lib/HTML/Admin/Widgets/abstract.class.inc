<?php

interface PXAdminWidgetIF {
	function html();
}

abstract class PXAdminWidget implements PXAdminWidgetIF{
	var $disable;
	var $parentWidget = null;

	public function __construct() {
	}

	function html() {
	}

	function __toString() {
		return $this->html();
	}

	function setParent($widget) {
		$this->parentWidget = $widget;
	}

	function getParentWidget() {
		if(is_null($this->parentWidget)) {
			$layout = PXRegistry::getLayout();
			$this->setParent($layout);
		}

		return $this->parentWidget;
	}

	function addToParent($label) {
		$parent = $this->getParentWidget();
		$this->addTo($label, $parent);
	}

	function setToParent($label) {
		$parent = $this->getParentWidget();
		$this->setTo($label, $parent);
	}

	function addTo($label, $widget) {
		$widget->add($label, $this);
	}

	function setTo($label, $widget) {
		$widget->set($label, $this);
	}

	function setDisabled($disable) {
		$this->disable = $disable ? ' disabled="disabled"' : '';
	}
}

abstract class PXAdminComplexWidget extends PXAdminWidget {
	var $blocks;
	public function __construct() {
		parent::__construct();
	}

	function append($block, $label) { // $block commonly used for instances of PXAdminWidget or PXAdminComplexWidget, but can apply string
		$this->blocks[$label][] = $block;
	}

	function decorate($type = null){
		if($type){
			PXDecorativeWidgetsCollection::fillByType($this, $type);
		} else {
			PXDecorativeWidgetsCollection::fill($this);
		}
	}
}
