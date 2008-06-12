<?php 
class PXStruct extends PXObjects {
	public $tree;
	public $format;

	public $base   = 'struct';
	public $pathId = array();
	public $rootId = -1;

	function findPath($urlPath) {
		$this->pathId    = $this->tree->getIdArrayByPath('pathname', $urlPath);
		$this->currentId = end($this->pathId);
		$this->rootId    = reset($this->pathId);
	}

	function hasCurrent() {
		return isset($this->tree->leafs[$this->currentId]);
	}

	function getCurrent() {
		return $this->tree->leafs[$this->currentId];
	}

	function load() {
		$this->tree = PXRegistry::getDb()->getObjects($this->format, true, DB_SELECT_TREE);
	}

	function getAllowedChilds() {
		return PXRegistry::getApp()->getAllowedChilds($this->format->id, $this->getCurrent());
	}
}
?>