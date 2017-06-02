<?php
// Используется для публикации события для пользователей планировщика (calendar)

class NLMailPublishEvent extends NLMailVEvent {

	public function __construct() {
		$this->_method = 'PUBLISH';

		$this->_requiredProps = [
			'DTStamp',
			'DTStart',
			'organizer',
			'summary',
			'uid'
		];

		$this->_uselessProps = [
			'attendee',
			'requestStatus'
		];

		parent::__construct();
	}
}
