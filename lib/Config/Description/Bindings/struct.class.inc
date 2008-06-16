<?php
class PXBindingDescriptionStruct extends PXBindingDescription {
	function isBinding() {
		if(!parent::isBinding()) {
			return false;
		}

		$struct = PXRegistry::getVar('struct');

		if(!$struct->hasCurrent()) {
			return false;
		}

		$current = $struct->getCurrent();

		return isset($current->content[$this->var]) && $current->content[$this->var] == $this->value;
	}
}
?>