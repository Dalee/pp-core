<?php
require_once('tabs.class.inc');
class PXWidgetMenuTabbed extends PXAdminComplexWidget {
	var $items;
	var $selected;
	const BEFORE_TABS = 0;
	const TABS        = 1;
	const AFTER_TABS  = 2;


	function PXWidgetMenuTabbed($items, $current, $getParam='area', $buildHref=true) {
		$this->items      = $items;
		$this->varName    = $getParam;
		$this->buildHref  = $buildHref;
		$this->selected   = $current;
		$this->blocks     = array();
	}

	function html() {
		$this->decorate();

		$tabs = new PXWidgetTabs($this->items, $this->buildHref);
		$tabs->setSelected($this->selected);
		$tabs->setVarName($this->varName);
		$this->append($tabs, self::TABS);

		$html = array();

		foreach($this->blocks as $b) {
			//	$html[] = sprintf('<div style="float: left; margin-top: -5px; text-align: left;">%s</div>', join($b));
			$html[] = join($b);
		}
		return join($html);
	}
}
?>
