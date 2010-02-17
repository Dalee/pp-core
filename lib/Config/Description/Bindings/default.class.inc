<?php
class PXBindingDescription {
	function __construct($rule, $pOrder) {
		$this->type   = (string) $rule['type'];
		$this->module = (string) $rule['module'];

		$this->var    = isset($rule['var']  ) ? (string) $rule['var']   : null;
		$this->value  = isset($rule['value']) ? (string) $rule['value'] : null;
		$this->order  = isset($rule['order']) ? (int) $rule['order']    : 0;

		$this->pOrder = (int) $pOrder;
		$this->finalizeQueue = false;
	}

	public function isBinding($content) {
		return isset(PXRegistry::getApp()->modules[$this->module]);
	}

	function getModuleDescription() {
		return PXRegistry::getApp()->modules[$this->module];
	}
	
	public function finalizeBindingsQueue(){
		$this->finalizeQueue = true;
	}
}
?>