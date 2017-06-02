<?php
// добавление одной или более сущностей к существующему событию, выполняется организатором (organizer)

class NLMailAddEvent extends NLMailVEvent {

	public function __construct() {
		$this->_method = 'ADD';

		$this->_requiredProps = [
			'DTStamp',
			'DTStart',
			'organizer',
			'sequence',
			'summary',
			'uid'
		];

		$this->_uselessProps = [
			'recurrenceId',
			'requestStatus'
		];

		parent::__construct();
	}
}
