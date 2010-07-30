<?php
	$status  = isset($this->helper->options->status) && in_array($this->helper->options->status, array("true", "false")) ? $this->helper->options->status : null;
	$dtype   = $this->app->types[$this->helper->objectType];
	$states  = array('true' => true, 'false' => false);
	if(isset($states[$status])){
		$status = $states[$status];
		foreach($this->helper->objectIds as $id){
			$object = $this->db->GetObjectById($dtype, $id);
			if(!$object || $object['status'] == $status)
				continue;
			
			$object['status'] = $status;
			$this->db->ModifyContentObject($dtype, $object);
		}
	} else {
		$RESULT['iserror'] = (object)array('notice' => 'Недопустимый статус объекта');
	}
?>
