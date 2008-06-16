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
		// "ÐÕÓÔÏÊ" ÐÒÏÅËÔ
		if(!isset($this->tree->levels[1])) {
			return;
		}

		// ÎÁÈÏÄÉÍ ÓÏÏÔ×ÅÔÓÔ×ÉÅ ÍÅÖÄÕ ÈÏÓÔÏÍ, ÎÁ ËÏÔÏÒÙÊ ÍÙ ÚÁÛÌÉ
		// É ËÏÒÎÅÍ × ÄÅÒÅ×Å, ËÏÔÏÒÙÊ ÍÏÖÅÔ ÂÙÔØ ÎÁÍ ÎÕÖÅÎ
		$hostAlias = 'default';
		if(isset($this->aliases[$host])) {
			$hostAlias = $this->aliases[$host];
		}

		// ÅÓÌÉ ÁÌÉÁÓ "ÎÅ ÎÁÛÌÉ", ÍÏÖÅÔ ÂÙÔØ ÎÕÖÎÙÊ ÈÏÓÔ ÅÓÔØ ÓÒÅÄÉ ËÏÒÎÅÊ?
		if($hostAlias == 'default') {
			foreach($this->tree->levels[1] as $_rootId) {
				if($this->tree->leafs[$_rootId]->content['pathname'] === $host) {
					$hostAlias = $host;
					break;
				}
			}
		}

		// ÎÁÈÏÄÉÍ ÎÕÖÎÙÊ ËÏÒÅÎØ É ÐÒÁ×ÉÍ ÅÇÏ pathname
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