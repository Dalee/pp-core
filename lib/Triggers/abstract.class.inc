<?php
abstract class PXAbstractTrigger {
	
	/**
	 * @var PXTriggerDescription
	 */
	protected $description;
	
	function __construct(PXTriggerDescription $description){
		$this->description = $description;
	}
}

?>