<?php
// Участник (attendee) отвечает на request события

class NLMailReplyEvent extends NLMailVEvent {

	public function __construct() {
		$this->_method = 'REPLY';
		$this->_requiredProps = [
			'attendee',
			'DTStamp',
			'organizer',
			'uid'
		];

		$this->_uselessProps = ['alarm'];
		parent::__construct();
	}
}
