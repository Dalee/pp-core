<?php
// Приглашения для событий, устанавливается расписание, место  и т.п 
require_once PPLIBPATH . 'Mail/EventCalendar/vevent.class.inc';
class NLMailRequestEvent extends NLMailVEvent {
	
	function NLMailRequestEvent() {
		$this->_method = 'REQUEST';
		
		$this->_requiredProps = array(
			'attendee',
			'DTStamp',
			'DTStart',
			'organizer',
			'summary',
			'uid'
		);
		
		$this->_uselessProps = array();
		
		$this->NLMailVEvent();
	}
}
?>