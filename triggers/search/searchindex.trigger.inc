<?php
class PXTriggerSearchSearchIndex extends PXAbstractSearchTrigger {
	function onIndexCronGetSelectCols($formatid, array &$cols) {
		$cols[] = 'status';

		if(PXRegistry::getTypes($formatid)->parent != null) {
			$cols[] = 'parent';
		}
	}

	private
	function _checkObject($data) {
		return isset($data['status']) && PXRegistry::getDb()->importBoolean($data['status']);
	}

	function onIndexCronCanIndexRow($formatid, &$row, &$tree) {
		if(!$this->_checkObject($row)) {
			return false;
		}

		return $this->checkParent($formatid, $row, $tree);
	}

	private
	function checkParent($formatid, &$row, &$tree) {
		$parent = PXRegistry::getTypes($formatid)->parent;
		$pId    = $row['parent'];

		while($parent != null) {
			$pType = PXRegistry::getTypes($parent);

			if($pType->id == 'struct') {
				if(!isset($tree->leafs[$pId])) {
					return false;
				}

				foreach($tree->getAncestors($pId) as $anc) {
					if(!$anc) {
						continue;
					}

					if(!isset($tree->leafs[$anc]) || !$this->_checkObject($tree->leafs[$anc]->content)) {
						return false;
					}
				}

				$parent = null;

			} else {
				if(!isset($row['parent']) || !is_numeric($row['parent'])) {
					return false;
				}

				$pObj = PXRegistry::getDb()->getObjectById($pType, $pId);

				if(!$this->_checkObject($pObj)) {
					return false;
				}

				$pId = $pObj['parent'];
				$parent = $pType->parent;
			}
		}

		return true;
	}
}
?>
