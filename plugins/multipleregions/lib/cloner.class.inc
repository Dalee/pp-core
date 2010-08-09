<?

class PXPublicRegionObjectCloner {
	public $db;
	public $app;

	const DENY_EDIT_FIELD = 'deny_region_edit';
	private $parentFormat  = null;

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

		if (isset($this->app->types[$format->parent])) {
			$this->parentFormat = $this->app->types[$format->parent];
		}
	}

	public function cloneObject($allowedRegionsIds) {/*{{{*/
		$this->denyEditValidate();

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
		$this->denyEditValidate();
		$this->substractRegions($allowedRegionsIds);
	}

	public function moveObject($allowedRegionsIds) {
		$this->cloneObject($allowedRegionsIds);
	}

	private function copyMainObject($allowedRegionsIds) {
		$objectInDB = $this->objectInDB;

		$reflex_field  = PXMultipleRegionsReflexer::REFLEX_FIELD;
		$regions_field = PXMultiRegions::REGION_MARK;

		$objectInDB[$reflex_field] = $this->oid;
		$objectInDB[$regions_field] = $allowedRegionsIds;

		if (isset($objectInDB['parent']) && $this->parentFormat) {
			$findClonedParent = $this->db->getObjectsByField($this->parentFormat,
				null, $reflex_field, $objectInDB['parent']);

			if (!empty($findClonedParent)) {
				$findClonedParent = reset($findClonedParent);
			}

			if (isset($findClonedParent['id'])) {
				$this->object['parent'] = $objectInDB['parent'] = $findClonedParent['id'];
			}
		}

		$this->nid = $this->object['id'] = 
			$this->db->cloneContentObject($this->format, $objectInDB, !empty($this->objectInDB['status']));

		$this->object[$regions_field] =
			array_intersect($this->object[$regions_field], $allowedRegionsIds);

		# set parent(donor) sys_order
		if (isset($objectInDB['sys_order'])) {
			$this->object['sys_order'] = $objectInDB['sys_order'];
		}

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

	private function denyEditValidate() {
		if(isset($this->objectInDB[self::DENY_EDIT_FIELD]) && !empty($this->objectInDB[self::DENY_EDIT_FIELD])) {
			$this->trigger->fireError('Только федеральное редактирование!');
		}
	}
}

?>
