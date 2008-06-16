<?php
class PXBindingDescriptionTypes extends PXBindingDescription {
	function isBinding() {
		if(!parent::isBinding()) {
			return false;
		}

		if($this->var !== 'id') {
			return false;
		}

		$struct = PXRegistry::getVar('struct');
		if($struct->hasType($this->value)) {
			return true;
		}

		$content = PXRegistry::getVar('content');
		if($content->hasType($this->value)) {
			return true;
		}

		$subContent = PXRegistry::getVar('subContent');
		if($subContent->hasType($this->value)) {
			return true;
		}

		return false;
	}
}
?>