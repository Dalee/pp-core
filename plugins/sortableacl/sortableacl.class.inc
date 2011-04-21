<?php

class PXPluginSortableAcl extends PXAbstractPlugin {
	function initialize($app) {
		$this->loadModule('sortableacl');
	}
}

?>
