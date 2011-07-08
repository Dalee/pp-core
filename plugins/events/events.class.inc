<?php

class PXPluginEvents extends PXAbstractPlugin {
	function initialize($app) {
		$this->load('lib/events_queue.class.inc');
		$this->loadCronrun('events');
	}
}

?>
