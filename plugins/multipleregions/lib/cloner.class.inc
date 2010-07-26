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
		$this->oid         = $object['id'];

		$this->trigger     = $trigger;

		$this->childTypes  = $format->childTypes();

		PXRegistry::assignToObject($this);
	}

	public function isValidObject($regions) {
		return isset($this->objectInDB['sys_regions']) 
				&& $regions->isInRegion('-1', $this->objectInDB['sys_regions']) 
					|| ($regions->isCurrentInSet($this->objectInDB['sys_regions']) 
						&& $this->objectInDB['sys_regions'] != array($this->rid));
	}

	private function isDenyEditValidate() {
		if(isset($this->objectInDB['deny_region_edit']) && !empty($this->objectInDB['deny_region_edit'])) {
			$this->trigger->fireError('Только федеральное редактирование!');
		}
	}

	public function cloneObject() {
		$this->isDenyEditValidate();
		$this->copyMainObject();
	}

	public function deleteObject() {
		$this->isDenyEditValidate();
		
		$tmp = $this->deletePublicAndSelfReg();

		$this->db->modifyingQuery(sprintf("UPDATE %s  SET sys_regions = '%s' where id = '%s'", 
			$this->format->id, PXMultiRegions::regToString((array) array_flip($tmp)), $this->objectInDB['id']));
	}

	private function deletePublicAndSelfReg() {
		$regionsIds = array_flip($this->objectInDB['sys_regions']);

		unset($regionsIds[-1]);
		unset($regionsIds[$this->rid]);

		return $regionsIds;
	}

	private function copyMainObject() {
		$object = $this->object;

		$object['reflex_id'] = $object['id'];
		$object['sys_regions'] = array();
		$object['copy'] = 1;
	
		$this->nid = $object['id'] = $this->db->cloneContentObject($this->format, $object, $status=true);

		$this->normalizeMainObject();
	}

	private function normalizeMainObject() {
		$regionsIds = $this->deletePublicAndSelfReg();

		$this->dbFields = array();
		$this->dbValues = array();

		foreach($this->objectInDB as $k => $i) {
			if(isset($this->object[$k])) {
				$params = array("db" => &$this->db, "dbFields" => &$this->dbFields, "dbValues" => &$this->dbValues);
					
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
