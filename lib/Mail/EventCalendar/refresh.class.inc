<?php
// Используется участником (attendee) для получения последней версии события

class NLMailRefreshEvent extends NLMailVEvent {
	
	function NLMailRefreshEvent() {
		$this->_method = 'REFRESH';
		
		$this->_requiredProps = array(
			'attendee',
			'DTStamp',
			'organizer',
			'uid'
		);
		
		$this->_uselessProps = array(
			'attach',
			'categories',
			'class',
			'contact',
			'created',
			'description',
			'DTEnd',
			'DTStart',
			'duration',
			'exeptionDate',
			'exceptionRule',
			'geo',
			'location',
			'priority',
			'recurrenceDate',
			'related',
			'requestStatus',
			'recurrenceRule',
			'sequence',
			'status',
			'summary',
			'transp',
			'url',
			'alarm'
		);
		
		$this->NLMailVEvent();
	}
}
?>