<?php

use PP\Plugin\AbstractPlugin;

class PXPluginSortableAcl extends AbstractPlugin {
	function initialize($app) {
		$this->loadModule('sortableacl');
	}
}

