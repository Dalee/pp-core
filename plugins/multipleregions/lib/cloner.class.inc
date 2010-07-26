<?

class PXPublicRegionObjectCloner {
	public $db;
	public $app;

	function __construct($format, &$object = null, &$objectInDB, &$dbFields = null, &$dbValues = null, $rid, $trigger) {
		$this->format      = $format;
	
		$this->object      =& $object;
	
		$this->objectInDB  =& $objectInDB;
	
		$this->dbFields    =& $dbFields;
		$this->dbValues    =& $dbValues;
	
		$this->rid         = $rid;
		$this->oid         = $objectInDB['id'];

		$this->trigger     = $trigger;

		$this->childTypes  = $format->childTypes();

		PXRegistry::assignToObject($this);
	}

	public function isValidObject($regions) {
		if(isset($this->objectInDB['sys_regions'])) {
				return $regions->isInRegion('-1', $this->objectInDB['sys_regions']);
					/* ($regions->isCurrentInSet($this->objectInDB['sys_regions']) 
					&& $this->objectInDB['sys_regions'] != array($this->rid))*/
		}

		return false;
	}

	private function isDenyEditValidate() {
		if(isset($this->objectInDB['deny_region_edit']) && !empty($this->objectInDB['deny_region_edit'])) {
			$this->trigger->fireError('Только федеральное редактирование!');
		}
	}

	public function cloneObject($to) {
		$this->isDenyEditValidate();
		$this->copyMainObject($to);
	}

	public function deleteObject() {
		$this->isDenyEditValidate();
		
		$tmp = $this->deletePublicAndSelfReg();

		$this->db->modifyingQuery(sprintf("UPDATE %s  SET sys_regions = '%s' where id = '%s'", 
			$this->format->id, PXMultiRegions::regToString((array) array_flip($tmp)), $this->oid));
	}

	public function moveObject() {
		$this->isDenyEditValidate();
		$tmp = $this->deletePublicAndSelfReg();
	}

	private function deletePublicAndSelfReg($to) {
		$regionsIds = array_flip($this->objectInDB['sys_regions']);

		foreach($to as $id) {
			if(isset($regionsIds[$id])) {
				unset($regionsIds[$id]);
			}
		}

		unset($regionsIds[-1]);
		var_dump($regionsIds);

		return (array) $regionsIds;
	}

	private function copyMainObject($to) {
		$object = $this->object;

		if(!is_array($object)) {
			$object = $this->db->getObjectById($this->format, $object);
		}

		$to = array_flip(array_keys($to));
		unset($to[""]);
		$to = array_flip($to);

		$object['reflex_id'] = $this->oid;
		$object['sys_regions'] = $to;
		$object['copy'] = 1;

		$this->nid = $object['id'] = $this->db->cloneContentObject($this->format, $object, $status=true);
		$this->normalizeMainObject($to);
	}

	private function normalizeMainObject($to) {
		$regionsIds = $this->deletePublicAndSelfReg($to);

		$this->dbFields = array();
		$this->dbValues = array();

		foreach($this->objectInDB as $k => $i) {
			if(isset($this->object[$k])) {
				$params = array("db" => &$this->db, "dbFields" => &$this->dbFields, "dbValues" => &$this->dbValues);

				if(!isset($this->format->fields[$k])) continue;
	
				$r = $this->format->fields[$k]
					->storageType->normalizeObjectAttribute($this->format->fields[$k], $this->objectInDB, $params);
			}
		}

		$this->setDbValue('sys_regions', PXMultiRegions::regToString(array_keys($regionsIds)));
	}

	private function setDbValue($key, $value) {
		$keys = array_flip($this->dbFields);

		if(!isset($keys[$key])) {
			$this->dbFields[] = $key;
			$this->dbValues[] = $value;
		} else {
			$this->dbValues[$keys[$key]] = $value;
		}
	}
}

?>
