<?php
class PXObjects extends PXContentObjects implements ArrayAccess, IteratorAggregate {
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

	// ArrayAccess if
	public function offsetGet($id) {
		return $this->data[$id];
	}

	public function offsetSet($id, $object) {
		$this->data[$id] = $object;
	}

	public function offsetExists ($id) {
		return isset($this->data[$id]);
	}

	public function offsetUnset($id) {
		unset($this->data[$id]);
	}

	/* IteratorAggregate */
	public function getIterator() {
		return new ArrayIterator($this->data);
	}
	/* /IteratorAggregate */
}
?>