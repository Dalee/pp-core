<?php
class PXTabbedMenuItems extends PXAdminComplexWidget {
	function html() {
		$parent =& $this->getParentWidget();
		$parent->items['contentversion'] = "Версии";
		return "";
	}
}
?>


