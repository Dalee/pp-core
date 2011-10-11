<?php
class PXBindingDescriptionStructType extends PXBindingDescriptionStruct {

	function __construct($rule, $pOrder) {
		parent::__construct($rule, $pOrder);
		$this->var   = 'type';
		$this->value = $this->module;
	}

}
?>