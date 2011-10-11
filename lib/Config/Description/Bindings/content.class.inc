<?php
class PXBindingDescriptionContent extends PXBindingDescription {
	protected $contentType = 'objects';

	protected function bindingRule($content, $current) {
		return parent::bindingRule($content, $current) && 
			isset($current[$this->var]) && $current[$this->var] == $this->value;
	}
}
?>