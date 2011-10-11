<?php
class PXBindingDescriptionStruct extends PXBindingDescription {
	protected $contentType = 'tree';

	protected function bindingRule($content, $current) {
		return parent::bindingRule($content, $current) && 
			isset($current->content[$this->var]) && $current->content[$this->var] == $this->value;
	}
}
?>