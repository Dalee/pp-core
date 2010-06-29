<?php

class PXAbstractPlugin {
	protected $name = null;
	public    $description;

	function __construct($app, $description) {
		$this->description = $description;
		$this->name = $description->getName();
		$this->initialize($app);
	}

	function initialize($app) {
	}

	function initSet($params = null) {
	}
}

?>
