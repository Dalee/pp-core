<?php
// Используется когда организатор (organizer) отменяет предложение (counter) участника (attendee)

class NLMailDeclineCounterEvent extends NLMailVEvent {

	public function __construct() {
		$this->_method = 'DECLINECOUNTER';

		$this->_requiredProps = array(
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

		parent::__construct();
	}
}

