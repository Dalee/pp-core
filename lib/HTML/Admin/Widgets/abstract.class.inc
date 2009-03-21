<?php
/*
interface PXAdminWidgetIF {
	function html();
}
*/

/*abstract*/
class PXAdminWidget /*implements PXAdminWidgetIF */{
	var $disable;

	function PXAdminWidget() {
	}

	function html() {
	}

	function setDisabled($disable) {
		$this->disable = $disable ? ' disabled="disabled"' : '';
	}
}

class PXAdminComplexWidget extends PXAdminWidget {
	var $blocks;

	function PXAdminComplexWidget() {
		$this->PXAdminWidget();
	}

	function append($block, $label) { // $block instance of PXAdminWidget or PXAdminComplexWidget
		$this->blocks[$label][] = $block;
	}
}
?>