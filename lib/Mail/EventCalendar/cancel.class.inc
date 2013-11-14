<?php
// отмена одной или более сущностей существующего события, выполняется организатором (organizer)

class NLMailCancelEvent extends NLMailVEvent {
	
	function NLMailCancelEvent() {
		$this->_method = 'CANCEL';
		
		$this->requiredMethods = array(
			'DTStamp',
			'organizer',
			'sequence',
			'uid'
		);
		$this->_uselessProps = array(
			'requestStatus',
			'alarm'
		);
		
		$this->NLMailVEvent();
	}
}
?>