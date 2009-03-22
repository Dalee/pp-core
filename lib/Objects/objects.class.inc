<?php
class PXObjects extends PXContentObjects {
	function hasCurrent() {
		return isset($this->data[$this->currentType][$this->currentId]);
	}

	function getCurrent() {
		return $this->data[$this->currentType][$this->currentId];
	}

	function getAllowedChilds() {
		return PXRegistry::getApp()->getAllowedChilds($this->currentType, $this->getCurrent());
	}

	function add($type, $objsArray) {
		$this->data[$type] = $objsArray;
	}

	function findCurrent($type, $pathname) {
		if (!isset(PXRegistry::getTypes($type)->fields['pathname'])) {
			return;
		}

		foreach ($this->data[$type] as $o) {
			if($o['pathname'] == $pathname) {
				$this->currentId   = $o['id'];
				$this->currentType = $type;

				return;
			}
		}
	}
}
?>