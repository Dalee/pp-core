<?php
class PXTriggerSearchMultipleregions extends PXAbstractSearchTrigger {
	private
	function isRegionalTypeOrParentIsReional($formatid) {
		$isReg = $this->isRegionalType($formatid);

		if($isReg) {
			return true;
		}

		$parent = PXRegistry::getTypes($formatid)->parent;
		if($parent !== null && $parent !== $formatid) {
			return $this->isRegionalType($parent);
		}

		return false;
	}

	private
	function isRegionalType($formatid) {
		return $this->regions()->isRegionalObject(PXRegistry::getTypes($formatid));
	}

	private
	function regions() {
		return PXMultiRegions::getInstance();
	}

	function onBeforeSearchStems($config, &$where) {
		$types = array();

		foreach($config as $dType => $fields) {
			$types[$this->isRegionalTypeOrParentIsReional($dType)][] = $dType;
		}

		$subWhere = array();

		if(isset($types[true]) && sizeof($types[true])) { // has multiregional types
			$rTypes = array_map(array(PXREgistry::getDB(), 'mapData'), $types[true]);
			$subWhere[] = "(".$this->regions()->findByRegion()." AND dtype IN (".implode(',', $rTypes)."))";
		}

		if(isset($types[false]) && sizeof($types[false])) { // has multiregional types
			$rTypes = array_map(array(PXREgistry::getDB(), 'mapData'), $types[false]);
			$subWhere[] = "dtype IN (".implode(',', $rTypes).")";
		}

		$where[] = '('.implode(' OR ', $subWhere).')';
	}

	function onIndexCronGetSelectCols($formatid, array &$cols) {
		if(PXRegistry::getTypes($formatid)->parent != null) {
			$cols[] = 'parent';
		}

		if ($this->isRegionalType($formatid)) {
			$cols[] = 'sys_regions';
		}
	}

	function onIndexCronStemWeight($formatid, array &$stemW, array $row) {
		if (isset($row[PXMultiRegions::REGION_MARK])) {
			$stemW[PXMultiRegions::REGION_MARK] = $row[PXMultiRegions::REGION_MARK];
		} else {
			$stemW[PXMultiRegions::REGION_MARK] = $this->voidRegion();
		}
	}

	private
	function voidRegion() {
		return '{}';
	}

	private
	function _checkObject($data) {
		return isset($data[PXMultiRegions::REGION_MARK]) && $data[PXMultiRegions::REGION_MARK] !== $this->voidRegion();
	}

	function onIndexCronCanIndexRow($formatid, &$row, &$tree) {
		if (!$this->isRegionalTypeOrParentIsReional($formatid)) {
			return true;
		}

		$checkParent = $this->checkParent($formatid, $row, $tree);
		if(!$checkParent) {
			return false;
		}

		return $this->_checkObject($row);
	}

	private
	function regArray($formatid, $object) {
		if(!isset($object[PXMultiRegions::REGION_MARK])) {
			return array();
		}

		$field = $this->_getRegionalFieldFromParent($formatid);

		$param = array();

		if(is_array($object[$field->name])) {
			return $object[$field->name];
		}

		return $field->storageType->normalize($field, $object, $param);
	}

	private
	function _getRegionalFieldFromParent($formatid) {
		if(!$this->isRegionalType($formatid)) {
			$parent = PXRegistry::getTypes($formatid)->parent;

			while($parent != null) {
				if($this->isRegionalType($parent)) {
					$formatid = $parent;
					break;
				}

				$parent = PXRegistry::getTypes($parent)->parent;
			}
		}

		return PXRegistry::getTypes($formatid)->fields[PXMultiRegions::REGION_MARK];
	}

	private
	function regString($formatid, $regs) {
		$field = $this->_getRegionalFieldFromParent($formatid);

		return $field->storageType->arrayToDB($regs);
	}

	private
	function checkParent($formatid, &$row, &$tree) {
		$parent = PXRegistry::getTypes($formatid)->parent;
		$pId    = $row['parent'];

		while($parent != null) {
			$pType = PXRegistry::getTypes($parent);

			if($pType->id == 'struct') {
				if ($this->isRegionalType($parent)) {
					if(!isset($tree->leafs[$pId])) {
						return false;
					}

					foreach(array_merge(array($pId), $tree->getAncestors($pId)) as $anc) {
						if(!$anc) {
							continue;
						}

						if(!isset($tree->leafs[$anc]) || !$this->_checkObject($tree->leafs[$anc]->content)) {
							return false;
						}

						$this->intersectParentRegions($formatid, $tree->leafs[$anc]->content, $row);
					}
				}

				$parent = null;

			} else {
				if ($this->isRegionalType($parent)) {
					if(!isset($row['parent']) || !is_numeric($row['parent'])) {
						return false;
					}

					$pObj = PXRegistry::getDb()->getObjectById($pType, $pId);

					if(!$this->_checkObject($pObj)) {
						return false;
					}

					$this->intersectParentRegions($formatid, $pObj, $row);
				}

				$pId = $pObj['parent'];
				$parent = $pType->parent;
			}
		}

		return true;
	}

	private
	function intersectParentRegions($formatid, $parent, &$row) {
		$oRegs  = $this->regArray($formatid, $row);

		if($this->isRegionalType($formatid) || sizeof($oRegs)) {
			$oRegs = array_intersect($parent[PXMultiRegions::REGION_MARK], $oRegs);
		} else {
			$oRegs = $parent[PXMultiRegions::REGION_MARK];
		}

		$row[PXMultiRegions::REGION_MARK] = $this->regString($formatid, $oRegs);
	}

}
?>