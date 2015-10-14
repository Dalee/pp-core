<?php

class PXTriggerDatabaseBlockcontent extends PXAbstractDatabaseTrigger {

	function OnDeleteObject(&$db, &$objId, &$format, &$objectInDB) {
		$hasInnerBlocks = false;

		foreach($format->fields as $field) {
			if(is_a($field->storageType, 'PXStorageTypeBlockcontent')) {
				$hasInnerBlocks = true;
				break;
			}
		}
		if(!$hasInnerBlocks) {
			return;
		}

		$db->query(sprintf("DELETE FROM block WHERE parent_type='%s' AND parent_id=%d", $db->EscapeString($format->id), $objId));
	}

}
