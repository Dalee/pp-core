<?php
class PXBindingDescriptionTypes extends PXBindingDescription {
	function isBinding($content) {
		if(!parent::isBinding($content)) {
			return false;
		}

		if($this->var !== 'id') {
			return false;
		}

		if($content->tree->hasType($this->value)) {
			return true;
		}

		if($content->objects->hasType($this->value)) {
			return true;
		}

		if($content->subObjects->hasType($this->value)) {
			return true;
		}

		return false;
	}
}
?>