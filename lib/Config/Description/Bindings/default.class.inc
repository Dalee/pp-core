<?php
class PXBindingDescription {
	protected $contentType;
	
	function __construct($rule, $pOrder) {
		$this->type   = (string) $rule['type'];
		$this->module = (string) $rule['module'];

		$this->var    = isset($rule['var']  ) ? (string) $rule['var']   : null;
		$this->value  = isset($rule['value']) ? (string) $rule['value'] : null;
		$this->order  = isset($rule['order']) ? (int) $rule['order']    : 0;

		$this->pOrder = (int) $pOrder;
	}

	public function getModuleDescription() {
		return PXRegistry::getApp()->modules[$this->module];
	}

	public function isBinding($content) {
		$current = null;
		
		if (!empty($this->contentType)) {
			if(!$content->{$this->contentType}->hasCurrent()) {
				return false;
			}
			
			$current = $content->{$this->contentType}->getCurrent();
		}

		return $this->bindingRule($content, $current);
	}

	protected function bindingRule($content, $current) {
		return isset(PXRegistry::getApp()->modules[$this->module]);
	}
}
?>