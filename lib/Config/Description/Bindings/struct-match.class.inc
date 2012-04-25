<?php
class PXBindingDescriptionStructMatch extends PXBindingDescriptionStruct {
	protected function bindingRule($content, $current) {
		return PXBindingDescription::bindingRule($content, $current) && 
			isset($current->content[$this->var]) && fnmatch($this->value, $current->content[$this->var]);
	}
}
?>