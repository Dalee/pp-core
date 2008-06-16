<?php
class PXBindingDescriptionSubContent extends PXBindingDescription {
	function isBinding() {
		if(!parent::isBinding()) {
			return false;
		}

		$subContent = PXRegistry::getVar('subContent');
		if(!$subContent->hasCurrent()) {
			return false;
		}

		$current = $subContent->getCurrent();
		return isset($current[$this->var]) && $current[$this->var] == $this->value;
	}
}
?>