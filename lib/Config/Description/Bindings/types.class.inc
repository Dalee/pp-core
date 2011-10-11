<?php
class PXBindingDescriptionTypes extends PXBindingDescription {

	protected function bindingRule($content, $current) {
		return parent::bindingRule($content, $current) && 
			$this->var == 'id' &&
			($content->tree->hasType($this->value) || 
				$content->objects->hasType($this->value) || 
				$content->subObjects->hasType($this->value));
	}
}
?>