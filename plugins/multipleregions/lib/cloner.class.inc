<?

class PXPublicRegionObjectCloner {
	public $db;
	public $app;

	const REFLEX_FIELD = 'sys_reflex_id';

	function __construct($format, &$object = null, &$objectInDB, &$dbFields = null, &$dbValues = null, $trigger) {
		$this->format      = $format;
		$this->object      =& $object;
		$this->objectInDB  =& $objectInDB;
		$this->dbFields    =& $dbFields;
		$this->dbValues    =& $dbValues;
		$this->oid         = $objectInDB['id'];
		$this->trigger     = $trigger;
		$this->childTypes  = $format->childTypes();

		PXRegistry::assignToObject($this);
	}

	private function isDenyEditValidate() {/*{{{*/
		if(isset($this->objectInDB['deny_region_edit']) && !empty($this->objectInDB['deny_region_edit'])) {
			$this->trigger->fireError('Только федеральное редактирование!');
		}
	}/*}}}*/

	public function cloneObject($allowedRegions) {/*{{{*/
		$this->isDenyEditValidate();

		$allowedRegions = array_keys($allowedRegions);

		$this->db->transactionBegin();
			$this->substractRegions($allowedRegions);
			$this->copyMainObject($allowedRegions);
		$this->db->transactionCommit();

		return $this->nid;
	}/*}}}*/

	private function substractRegions($allowedRegions) {
		$regionsIds = array_flip($this->objectInDB[PXMultiRegions::REGION_MARK]);

		foreach($allowedRegions as $id) {
			if(isset($regionsIds[$id])) {
				unset($regionsIds[$id]);
			}
		}

		$this->db->modifyingQuery($s = sprintf("UPDATE %s set %s = '%s' where id = '%s'", 
			$this->format->id, PXMultiRegions::REGION_MARK, PXMultiRegions::regToString(array_flip($regionsIds)), $this->oid));
	}

	public function deleteObject($allowedRegions) {
		$this->isDenyEditValidate();
		$this->substractRegions($allowedRegions);
	}

	public function moveObject($allowedRegions) {
		$this->cloneObject($allowedRegions);
	}

	private function copyMainObject($allowedRegions) {
		$donorObject = $this->objectInDB;
		$object = $this->object;

		if(!is_array($object)) {
			$object = $this->db->getObjectById($this->format, $object);
		}

		$object[self::REFLEX_FIELD] = $this->oid;
		$object[PXMultiRegions::REGION_MARK] = $allowedRegions;

		#fixme!!!
		$this->nid = $object['id'] = @$this->db->cloneContentObject($this->format, $object, !empty($object['status']));

		#fixme ripped cloneContentObject
		foreach ($this->format->fields as $k => $v) {
			$p = array('donor'=> $donorObject, 'format'=> $this->format->id);
			$v->storageType->cloneFile($v, $object, $p);
		}
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
