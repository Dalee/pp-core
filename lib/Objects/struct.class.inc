<?php

use PP\Lib\Objects\ContentObjectsInterface;

/* TODO: move this code to NLLeaf */
class PXStructLeaf implements Iterator {
	private $data, $struct;

	function __construct($leaf, $struct) {
		$this->data   = $leaf;
		$this->struct = $struct;
	}

	function __call($name, $arguments) {
		if(method_exists($this->data, $name)) {
			return call_user_func_array(array($this->data, $name), $arguments);

		} else {
			$trace = PXTrace(); // TODO: refactor
			FatalError('Call to undefined method '.__CLASS__.'::'.$name.'() in '.$trace[1]['file'].' on line '.$trace[1]['line']);
		}
	}

	function parent($level=1) {
		$parentId = is_object($this->data) ? $this->data->parent($level) : null;
		return is_null($parentId) ? new PXStructLeaf404() : $this->struct->getLeaf($this->data->tree->leafs[$parentId]);
	}

	function __get($key) {
		if(isset($this->data->{$key})) {
			return $this->data->{$key};
		}
		if (!isset($this->data->content[$key])) {
			return null;
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

		return $this->struct->getLeaf($this->data->tree->leafs[$key]);
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
		$this->id = -1;
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

class PXTreeObjects implements ContentObjectsInterface, ArrayAccess, Iterator {
	public $format;
	public $tree; //FIXME: really public ? Has some direct calls from PXStructLeaf.

	public $pathId = array();
	public $rootId = -1;
	public $currentId;

	private $leafsCache = array();
	private $treeIsLoaded = false;

	function __construct() {
		$this->tree = PXRegistry::getDb()->buildTree(array());
	}

	function __call($name, $arguments) {
		if(method_exists($this->tree, $name)) {
			return call_user_func_array(array($this->tree, $name), $arguments);
		} else {
			$trace = pxtrace();
			FatalError('Call to undefined method '.__CLASS__.'::'.$name.'() in '.$trace[1]['file'].' on line '.$trace[1]['line']);
		}
	}

	function __get($key) {
		$conds = array();
		switch($key) {
			case 'root':
				$conds['root'] = array($this->rootId !== -1, $this->rootId);
			case 'current':
				$conds['current'] = array($this->hasCurrent(), $this->currentId);

				if($conds[$key][0]) {
					return $this->getLeaf($this->tree->leafs[$conds[$key][1]]);
				}

				return new PXStructLeaf404();
			default:
				if(isset($this->tree->$key)) {
					return $this->tree->$key;
				}
		}

		return null;
	}

	function __isset($key) {
		return isset($this->$key) || isset($this->tree->$key);
	}

	function getLeaf($leafArray) {
		if(!isset($this->leafsCache[$leafArray->id])){
			$this->leafsCache[$leafArray->id] = new PXStructLeaf($leafArray, $this);
		}

		return $this->leafsCache[$leafArray->id];
	}


	function currentType() {
		if(!$this->hasCurrent()) {
			return;
		}

		$current = $this->getCurrent();
		return $current->content['type'];
	}

	public
	function setCurrent($id) {
		$this->currentId = $id;
	}

	public
	function setCurrentType($type) {
		if(!$this->hasCurrent()) {
			return false;
		}

		$this->tree->leafs[$this->currentId]->content['type'] = $type;

		return true;
	}

	function hasCurrent() {
		return isset($this->tree->leafs[$this->currentId]);
	}

	function getCurrent() {
		return $this->getLeaf($this->tree->leafs[$this->currentId]);
	}

	function getCurrentType(){
		return $this->currentType();
	}

	// FIXME !!!
	function getLinks() {
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
				$this->tree->leafs[$_rootId]->content['__pathname'] = $this->tree->leafs[$_rootId]->content['pathname'];
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
		$this->treeIsLoaded = true;
	}

	function isLoaded() {
		return $this->treeIsLoaded;
	}

	function getAllowedChilds() {
		$cur = $this->getCurrent();
		return PXRegistry::getApp()->getAllowedChilds($this->format->id, $cur);
	}

	function hasType($type) {
		return isset($this->format->id) && $type === $this->format->id && !empty($this->tree->leafs);
	}

	// ArrayAccess if
	public function offsetGet($id) {
		return $this->getLeaf($this->tree->leafs[$id]);
	}

	public function offsetSet($id, $object) {
		$this->tree->leafs[$id] = $object;
	}

	public function offsetExists ($id) {
		return isset($this->tree->leafs[$id]);
	}

	public function offsetUnset($id) {
		unset($this->tree->leafs[$id]);
		unset($this->leafsCache[$id]);
	}

	/* Iterator */
	function rewind() {
		reset($this->tree->leafs);
	}

	function current() {
		return $this->getLeaf($this->tree->leafs[$this->key()]);
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
