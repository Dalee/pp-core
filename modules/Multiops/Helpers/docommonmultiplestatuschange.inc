<?php
	$objects = join(',', $this->helper->objectIds);
	$status  = isset($this->helper->options->status) && in_array($this->helper->options->status, array("true", "false")) ? $this->helper->options->status : null;
	if(is_string($status)){
		$this->db->modifyingQuery(<<<SQL
			UPDATE 
				{$this->helper->objectType}
			SET
				status = {$status}
			WHERE
				id IN ({$objects})
SQL
		);
	} else {
		$RESULT['iserror'] = (object)array('notice' => 'Недопустимый статус объекта');
	}
?>