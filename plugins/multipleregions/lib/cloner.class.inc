<?

class PXPublicRegionObjectCloner {
	public $db;
	public $app;
	
	const DENY_EDIT_FIELD = 'deny_region_edit';

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

	public function cloneObject($allowedRegionsIds) {/*{{{*/
		$this->isDenyEditValidate();

		$this->db->transactionBegin();
			$this->substractRegions($allowedRegionsIds);
			$this->copyMainObject($allowedRegionsIds);
		$this->db->transactionCommit();

		return $this->nid;
	}/*}}}*/

	private function substractRegions($allowedRegionsIds) {
		$regionsIds = array_flip($this->objectInDB[PXMultiRegions::REGION_MARK]);

		foreach($allowedRegionsIds as $id) {
			if(isset($regionsIds[$id])) {
				unset($regionsIds[$id]);
			}
		}

		$this->db->modifyingQuery(
			sprintf("UPDATE %s set %s = '%s' where id = '%s'", 
				$this->format->id, 
				PXMultiRegions::REGION_MARK, 
				PXMultipleRegionsHelper::toString(array_flip($regionsIds)),
				$this->oid
			)
		);
	}

	public function deleteObject($allowedRegionsIds) {
		$this->isDenyEditValidate();
		$this->substractRegions($allowedRegionsIds);
	}

	public function moveObject($allowedRegionsIds) {
		$this->cloneObject($allowedRegionsIds);
	}

	private function copyMainObject($allowedRegionsIds) {
		$objectInDB = $this->objectInDB;

		$objectInDB[PXMultipleRegionsReflexer::REFLEX_FIELD] = $this->oid;
		$objectInDB[PXMultiRegions::REGION_MARK] = $allowedRegionsIds;

		$findClonedParent = $this->db->query(
			sprintf("select id from %s where %s = '%s'",
				$this->format->id, 
				PXMultipleRegionsReflexer::REFLEX_FIELD,
				$objectInDB['parent']
			)
		);

		$findClonedParent = pos((array) $findClonedParent);

		if(isset($findClonedParent['id'])) {
			$objectInDB['parent'] = $findClonedParent['id'];
		}

		#create clone
		$this->nid = $this->object['id'] = 
			$this->db->cloneContentObject($this->format, $objectInDB, !empty($this->objectInDB['status']));

		#set user modify
		$this->db->modifyContentObject($this->format, $this->object);
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

	private function isDenyEditValidate() {/*{{{*/
		if(isset($this->objectInDB[self::DENY_EDIT_FIELD]) && !empty($this->objectInDB[self::DENY_EDIT_FIELD])) {
			$this->trigger->fireError('Только федеральное редактирование!');
		}
	}/*}}}*/
}

?>
