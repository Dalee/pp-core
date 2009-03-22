<?php
abstract class PXContentObjects implements ArrayAccess, IteratorAggregate  {
	protected $data, $links, $linkedObjs; //FIXME
	protected $currentId, $currentType;

	function __construct() {
		$this->currentId   = -1;
		$this->currentType = -1;

		$this->data        = array();
		$this->links       = array();
		$this->linkedObjs  = array();
	}

	function sizeof() {
		return sizeof($this->data);
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

	// IteratorAggregate if
	public function getIterator() {
		return new ArrayIterator($this->data);
	}

	abstract function hasCurrent();
	abstract function getCurrent();
	abstract function getAllowedChilds();

	function hasType($type) {
		return array_key_exists($type, $this->data) && sizeof($this->data[$type]);
	}

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

			$objsArray = array();

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
?>