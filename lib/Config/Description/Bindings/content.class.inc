<?php
class PXBindingDescriptionContent extends PXBindingDescription {
	function isBinding() {
		if(!parent::isBinding()) {
			return false;
		}

		$content = PXRegistry::getVar('content');
		if(!$content->hasCurrent()) {
			return false;
		}

		$current = $content->getCurrent();
		return isset($current[$this->var]) && $current[$this->var] == $this->value;
	}
}
?>