<?php
class PXBindingDescriptionStructType extends PXBindingDescription {
	function isBinding($content) {
		if(!parent::isBinding($content)) {
			return false;
		}

		if(!$content->tree->hasCurrent()) {
			return false;
		}

		$current = $content->tree->getCurrent();

		return isset($current->content['type']) && $current->content['type'] == $this->module;
	}
}
?>