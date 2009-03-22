<?php
class PXBindingDescriptionSubContent extends PXBindingDescription {
	function isBinding($content) {
		if(!parent::isBinding($content)) {
			return false;
		}

		if(!$content->subobjects->hasCurrent()) {
			return false;
		}

		$current = $content->subobjects->getCurrent();
		return isset($current[$this->var]) && $current[$this->var] == $this->value;
	}
}
?>