<?php
// Приглашения для событий, устанавливается расписание, место  и т.п
require_once PPLIBPATH . 'Mail/EventCalendar/vevent.class.inc';

class NLMailRequestEvent extends NLMailVEvent {

	public function __construct() {
		$this->_method = 'REQUEST';
		$this->_requiredProps = [
			'attendee',
			'DTStamp',
			'DTStart',
			'organizer',
			'summary',
			'uid'
		];

		$this->_uselessProps = [];
		parent::__construct();
	}
}
