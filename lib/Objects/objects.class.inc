<?php
class PXObjects extends PXContentObjects {
	public $currentId, $currentType;
	
	function hasCurrent() {
		return @isset($this[$this->currentType][$this->currentId]);
	}

	function getCurrent() {
		return $this[$this->currentType][$this->currentId];
	}

	function getAllowedChilds() {
		return PXRegistry::getApp()->getAllowedChilds($this->currentType, $this->getCurrent());
	}

	function add($type, $objsArray) {
		$this[$type] = $objsArray;
	}

	function __get($key) {
		switch($key) {
			case 'current':
				if($this->hasCurrent()) {
					return $this->getCurrent();
				} else {
					return null;
				}
				break;
		}

		if(isset($this->$key)) {
			return $this->$key;
		}

		return null;
	}

	public
	function setCurrent($id, $type) {
		$this->currentId   = $id;
		$this->currentType = $type;
	}

	function findCurrent($type, $pathname) {
		if (!isset(PXRegistry::getTypes($type)->fields['pathname'])) {
			return;
		}

		foreach ($this[$type] as $o) {
			if($o['pathname'] == $pathname) {
				$this->setCurrent($o['id'], $type);
				return;
			}
		}
	}
}
?>