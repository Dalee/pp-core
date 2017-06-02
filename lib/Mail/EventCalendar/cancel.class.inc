<?php
// отмена одной или более сущностей существующего события, выполняется организатором (organizer)

class NLMailCancelEvent extends NLMailVEvent {

	public function __construct() {
		$this->_method = 'CANCEL';
		$this->requiredMethods = [
			'DTStamp',
			'organizer',
			'sequence',
			'uid'
		];

		$this->_uselessProps = [
			'requestStatus',
			'alarm'
		];

		parent::__construct();
	}
}
