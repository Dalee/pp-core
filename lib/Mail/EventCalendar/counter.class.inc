<?php
// Используется когда участник (attendee) хочет внести предложения либо изменить, например время события и т.п

class NLMailCounterEvent extends NLMailVEvent {

	public function __construct() {
		$this->_method = 'COUNTER';

		$this->_requiredProps = [
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
