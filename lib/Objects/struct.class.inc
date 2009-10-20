<?php 
/* TODO: move this code to NLLeaf */
class PXStructLeaf implements Iterator {
	private $data, $struct;

	function __construct($leaf, $struct) {
		$this->data   = $leaf;
		$this->struct = $struct;
	}

	function __get($key) {
		if($key == 'content') {
			return $this->data->content;
		}

		return $this->data->content[$key];
	}

	function path() {
		return $this->data->tree->getFullPathString($this->data->id);
	}

	function isCurrent() {
		return $this->data->id == $this->struct->currentId;
	}

	function isRoot() {
		return $this->data->id == $this->struct->rootId;
	}

	function hasChilds() {
		return sizeof($this->data->children) >  0;
	}


	function createpath() {
		return createPathByParentId($this->struct->tree, $this->id);
	}

	/* Iterator methods */
	public function valid() {
		return $this->current() !== false;
	}

	public function rewind() {
		reset($this->data->children);
	}

	public function current() {
		$key = current($this->data->children);

		if($key === false) {
			return false;
		}

		return new PXStructLeaf($this->data->tree->leafs[$key], $this->struct);
	}

	public function key() {
		return key($this->data->children);
	}

	public function next() {
		$key = next($this->data->children);
		return $key;
	}

	/* /Iterator methods */
}

class PXStructLeaf404 extends PXStructLeaf {
	function __construct() {
	}

	function __get($key) {
		return null;
	}

	function path() {
		return null;
	}

	function isCurrent() {
		return false;
	}

	function isRoot() {
		return false;
	}

	function hasChilds() {
		return false;
	}

	public function valid() {
		return false;
	}

	public function rewind() {
	}

	public function current() {
		return false;
	}

	public function key() {
		return false;
	}

	public function next() {
		return false;
	}
}

class PXTreeObjects extends PXContentObjects implements ArrayAccess, Iterator {
	public $format;

	public $pathId = array();
	public $rootId = -1;
	public $currentId;

	function __get($key) {
		switch($key) {
			case 'root':
				return new PXStructLeaf($this->tree->leafs[$this->rootId], $this);
				break;

			case 'current':
				if($this->hasCurrent()) {
					return new PXStructLeaf($this->tree->leafs[$this->currentId], $this);
				} else {
					return new PXStructLeaf404();
				}
				break;
		}

		if(!isset($this->$key) && isset($this->tree->$key))
			return $this->tree->$key;

		return null;
	}

	function currentType() {
		if(!$this->hasCurrent()) {
			return;
		}

		$current = $this->getCurrent();
		return $current->content['type'];
	}

	function hasCurrent() {
		return isset($this->tree->leafs[$this->currentId]);
	}

	function getCurrent() {
		return $this->tree->leafs[$this->currentId];
	}

	function findCurrent($urlPath) {
		// Guessing context
		$this->_findRoot($urlPath[0]);

		$this->pathId    = $this->tree->getIdArrayByPath('pathname', $urlPath);

		$this->currentId = end($this->pathId);
		$this->rootId    = reset($this->pathId);
	}

	function _findRoot($host) {
		// "пустой" проект
		if(!isset($this->tree->levels[1])) {
			return;
		}

		// находим соответствие между хостом, на который мы зашли
		// и корнем в дереве, который может быть нам нужен
		$hostAlias = 'default';
		if(isset($this->aliases[$host])) {
			$hostAlias = $this->aliases[$host];
		}

		// если алиас "не нашли", может быть нужный хост есть среди корней?
		if($hostAlias == 'default') {
			foreach($this->tree->levels[1] as $_rootId) {
				if($this->tree->leafs[$_rootId]->content['pathname'] === $host) {
					$hostAlias = $host;
					break;
				}
			}
		}

		// находим нужный корень и правим его pathname
		foreach($this->tree->levels[1] as $_rootId) {
			if($this->tree->leafs[$_rootId]->content['pathname'] === $hostAlias) {
				$this->tree->leafs[$_rootId]->content['pathname'] = $host;
				break;
			}
		}
	}

	function setAliases($domainAliases) {
		$this->aliases = $domainAliases;
	}

	function setFormat($type) {
		$this->format = PXRegistry::getTypes($type);
	}

	function load($urlPath) {
		$this->tree = PXRegistry::getDb()->getObjects($this->format, true, DB_SELECT_TREE);

		$this->findCurrent($urlPath);
	}

	function getAllowedChilds() {
		return PXRegistry::getApp()->getAllowedChilds($this->format->id, $this->getCurrent());
	}

	function hasType($type) {
		return $type === $this->format->id && sizeof($this->tree->leafs);
	}

	// ArrayAccess if
	public function offsetGet($id) {
		return $this->tree->leafs[$id];
	}

	public function offsetSet($id, $object) {
		$this->tree->leafs[$id] = $object;
	}

	public function offsetExists ($id) {
		return isset($this->tree->leafs[$id]);
	}

	public function offsetUnset($id) {
		unset($this->tree->leafs[$id]);
	}

	/* Iterator */
	function rewind() {
		reset($this->tree->leafs);
	}

	function current() {
		$leaf = new PXStructLeaf($this->tree->leafs[$this->key()], $this); 
		return $leaf;
	}

	function key() {
		return key($this->tree->leafs);
	}

	function next() {
		next($this->tree->leafs);
	}

	function valid() {
		$current = key($this->tree->leafs);
		return isset($this->tree->leafs[$current]);
	}
	/* /Iterator */
}
?>
