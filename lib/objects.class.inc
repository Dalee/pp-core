<?php 
abstract class PXObjects {
	protected $data, $links, $linkedObjs;

	public $currentId, $currentType, $base;

	function __construct() {
		$this->currentId   = -1;
		$this->currentType = -1;

		$this->data        = array();
		$this->links       = array();
		$this->linkedObjs  = array();
	}

	abstract function hasCurrent();
	abstract function getCurrent();
	abstract function getAllowedChilds();

	function loadChilds($childs, $pathName) {
		if(!$this->hasCurrent()) {
			return;
		}

		$allowed = $this->getAllowedChilds();

		if (!count($allowed)) {
			return array();
		}

		foreach ($allowed as $type=>$behaviour) {
			$format = PXRegistry::getTypes($type);

			if(!is_object($format)) {
				continue;
			}

			switch ($behaviour) {
				case PP_CHILDREN_FETCH_NONE:
					break;

				case PP_CHILDREN_FETCH_SELECTED:
					if (!isset($format->fields['pathname'])) {
						break(2);
					}

					$objsArray = PXRegistry::getDb()->getObjectsByFieldLimited($format, true, array('pathname' => $pathName, 'parent' => $this->currentId), 'IGNORED', 1, 0);
					break;

				case PP_CHILDREN_FETCH_ALL:
					$objsArray = PXRegistry::getDb()->getObjectsByParent($format, true, $this->currentId);
					break;
			}

			$childs->add($type, $objsArray);
			$childs->findCurrent($type, $pathName);
		}
	}

	function getLinks() {
		if(!$this->hasCurrent()) {
			return;
		}

		$format = PXRegistry::getTypes($this->currentType);

		if(!isset($format->references)) {
			return;
		}

		$db = PXRegistry::getDb();
		foreach ($format->references as $ref) {
			if($ref->load == 'manual') {
				continue;
			}

			$otherType   = $ref->getOther($format->id);
			$otherFormat = PXRegistry::getTypes($otherType);

			$links = $db->getLinks($ref, $format->id, $this->currentId);

			$this->links[$ref->name] = $links;
			$this->linkedObjs[$otherType] = $db->getObjectsByIdArray($otherFormat, true, array_keys($links));
		}
		
	}
}

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

class PXContent extends PXObjects {
	public $base = 'content';

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

class PXSubContent extends PXContent {
	public $base   = 'subcontent';

	function getAllowedChilds() {
		FatalError('I can\'t have children anymore!!!!');
	}
}
/*
class PXContentDirectory extends PXContent {
}
*/

?>