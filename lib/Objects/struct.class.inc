<?php 
class PXStruct extends PXObjects {
	public $tree;
	public $format;

	public $base   = 'struct';
	public $pathId = array();
	public $rootId = -1;

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

	/* Iterator methods */
	public function rewind() {
		reset($this->tree->leafs);
	}

	public function current() {
		return current($this->tree->leafs);
	}

	public function key() {
		return key($this->tree->leafs);
	}

	public function next() {
		return next($this->tree->leafs);
	}

	/* /Iterator methods */
}
?>