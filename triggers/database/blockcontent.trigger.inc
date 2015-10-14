<?php

class PXTriggerDatabaseBlockcontent extends PXAbstractDatabaseTrigger {

	protected static $TYPE = 'block';

	public /*override*/ function OnBeforeAddObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues) {
		if($format->id != self::$TYPE) {
			return true;
		}
		$parentFormat = &$db->app->types[$object['parent_type']];
		if(!isset($parentFormat) || !isset($parentFormat->fields[$object['parent_field']])) {
			return false;
		}
		if(!isset($object['parent_id']) || empty($object['parent_id'])) {
			return false;
		}
		return true;
	}

	public /*override*/ function OnDeleteObject(&$db, &$objId, &$format, &$objectInDB) {
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

		$db->query(sprintf("DELETE FROM %s WHERE parent_type='%s' AND parent_id=%d", self::$TYPE, $db->EscapeString($format->id), $objId));
	}

}
