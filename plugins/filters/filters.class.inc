<?php

class PXPluginFilters extends PXAbstractPlugin {
	function initialize($app) {
		$this->loadTrigger("database/filters");
		
		$this->loadOnlyInAdmin("lib/widget.class.inc");
		$this->loadOnlyInAdmin("decorators.php");
	}
}

?>
