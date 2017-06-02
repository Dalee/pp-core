<?php
// Используется участником (attendee) для получения последней версии события

class NLMailRefreshEvent extends NLMailVEvent {

	public function __construct() {
		$this->_method = 'REFRESH';
		$this->_requiredProps = [
			'attendee',
			'DTStamp',
			'organizer',
			'uid'
		];

		$this->_uselessProps = [
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
		];

		parent::__construct();
	}
}
