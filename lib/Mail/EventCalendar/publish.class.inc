<?php
// Используется для публикации события для пользователей планировщика (calendar)

class NLMailPublishEvent extends NLMailVEvent {
	
	function NLMailPublishEvent() {
		$this->_method = 'PUBLISH';
		
		$this->_requiredProps = array(
			'DTStamp',
			'DTStart',
			'organizer',
			'summary',
			'uid'
		);
		
		$this->_uselessProps = array(
			'attendee',
			'requestStatus'
		);
		
		$this->NLMailVEvent();
	}
}
?>