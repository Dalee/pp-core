<?php
class PXTriggerSearchMultipleregions extends PXAbstractSearchTrigger {
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
			$types[$this->isRegionalType($dType)][] = $dType;
		}

		$subWhere = array();

		if(sizeof($types[true])) { // has multiregional types
			$rTypes = array_map(array(PXREgistry::getDB(), 'mapData'), $types[true]);
			$subWhere[] = "(".$this->regions()->findByRegion()." AND dtype IN (".implode(',', $rTypes)."))";
		}

		if(sizeof($types[false])) { // has multiregional types
			$rTypes = array_map(array(PXREgistry::getDB(), 'mapData'), $types[false]);
			$subWhere[] = "dtype IN (".implode(',', $rTypes).")";
		}

		$where[] = '('.implode(' OR ', $subWhere).')';
	}

	function onBeforeSearchObjects($formatid, array &$where) {
		return;

		if (PXRegistry::getRequest()->isSetVar('search_by_all_regions')) {
			return;
		}

		if ($this->isRegionalType($formatid)) {
			$where[] = $this->regions()->findByRegion();
		}
	}

	function onIndexCronGetSelectCols($formatid, array &$cols) {
		if ($this->isRegionalType($formatid)) {
			$cols[] = 'sys_regions';
		}
	}

	function onIndexCronStemWeight($formatid, array &$stemW, array $row) {
		if ($this->isRegionalType($formatid)) {
			$stemW[PXMultiRegions::REGION_MARK] = $row[PXMultiRegions::REGION_MARK];
		} else {
			$stemW[PXMultiRegions::REGION_MARK] = '{}';
		}
	}
}
?>