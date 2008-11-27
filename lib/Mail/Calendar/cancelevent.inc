<?php
require_once('vevent.class.inc');

class NLMailCancelEvent extends NLMailVEvent {
	
	function NLMailCancelEvent() {
		$this->_method = 'CANCEL';
		
		$this->requiredMethods = array(
			'_DTStamp',
			'_organizer',
			'_sequence',
			'_uid'
		);

		$this->NLMailVEvent();
	}
	
	
}
?>