<?php

use PP\Plugin\PluginAbstract;

class PXPluginEvents extends PluginAbstract {
	function initialize($app) {
		$this->load('lib/events_queue.class.inc');
		$this->loadCronrun('events');
	}
}
