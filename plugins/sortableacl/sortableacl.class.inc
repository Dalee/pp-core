<?php

use PP\Plugin\PluginAbstract;

class PXPluginSortableAcl extends PluginAbstract {
	function initialize($app) {
		$this->loadModule('sortableacl');
	}
}

