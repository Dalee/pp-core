<?php
require_once('vevent.class.inc');

class NLMailRequestEvent extends NLMailVEvent {
	
	function NLMailRequestEvent() {
		$this->_method = 'REQUEST';
		
		$this->requiredMethods = array(
			'_attendee',
			'_DTStamp',
			'_DTStart',
			'_organizer',
			'_summary',
			'_uid'
		);
		
		$this->NLMailVEvent();
	}
	
	
}
?>