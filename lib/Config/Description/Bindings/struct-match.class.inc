<?php
class PXBindingDescriptionStructMatch extends PXBindingDescriptionStruct {
	protected function bindingRule($content, $current) {
		return PXBindingDescription::bindingRule($content, $current) && 
			isset($current->content[$this->var]) && preg_match($this->value, $current->content[$this->var]);
	}
}
?>