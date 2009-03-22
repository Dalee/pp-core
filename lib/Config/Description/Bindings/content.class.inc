<?php
class PXBindingDescriptionContent extends PXBindingDescription {
	function isBinding($content) {
		if(!parent::isBinding($content)) {
			return false;
		}

		if(!$content->objects->hasCurrent()) {
			return false;
		}

		$current = $content->objects->getCurrent();
		return isset($current[$this->var]) && $current[$this->var] == $this->value;
	}
}
?>