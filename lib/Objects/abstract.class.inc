<?php
abstract class PXContentObjects {
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

	abstract function hasCurrent();
	abstract function getCurrent();
	abstract function getAllowedChilds();

	function hasType($type) {
		return array_key_exists($type, $this->data) && sizeof($this->data[$type]);
	}

	function getCurrentType(){
		return $this->currentType;
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