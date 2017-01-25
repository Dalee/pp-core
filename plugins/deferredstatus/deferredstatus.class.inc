<?php

use PP\Plugin\PluginAbstract;

class PXPluginDeferredStatus extends PluginAbstract {

	function initialize ($app) {
		$this->load('lib/deferredstatus_helper.class.inc');
		PXDeferredStatusHelper::init();

		$this->loadTrigger('database/deferredstatus');
		$this->loadCronrun('deferredstatus');
		$this->loadDisplayType('sysdeferredlist');
	}

}
