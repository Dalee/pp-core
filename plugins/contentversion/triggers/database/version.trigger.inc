<?php
class PXTriggerDatabaseVersion extends PXAbstractDatabaseTrigger {
	function OnBeforeAddObject(&$db, &$newId, &$format, &$object, &$dbFields, &$dbValues) {
		if(!isset($format->fields['noversion'])) {
			$dbFields[] = 'sys_version';
			$dbValues[] =  isset($object['sys_version']) ? $object['sys_version'] : null;
			$dbFields[] = 'sys_modifyer';
			$dbValues[] =  isset($object['sys_modifyer']) ? $object['sys_modifyer'] : '';
			$dbFields[] = 'sys_original';
			$dbValues[] = '';
		}
		return true;
	}

	function OnAddObject(&$db, &$newId, &$format, &$object, &$dbFields, &$dbValues) {
		if(!isset($format->fields['noversion'])) {
			$db->ModifyingQuery(sprintf("UPDATE %s SET sys_original = %s WHERE id = %d", $format->id,  (isset($object['sys_original']) ? $object['sys_original'] : 'id'), $newId));
		}
	}

	function OnBeforeGetDataBuildQuery($db, $format, &$mode, &$selector, &$where, &$order, &$limit){
		if(!isset($format->fields['noversion'])) {
			if($mode != DB_SELECT_COUNT) {
				$selector .= ', '.$format->id.'.sys_original, '.$format->id.'.sys_modifyer, '.$format->id.'.sys_version';
			}
			if(strpos($where, $format->id.'.id') === false) {
				$where = strlen($where) ? "$where AND sys_version IS NULL" : "sys_version IS NULL";
			}
		}
	}

	function OnBeforeDeleteObject(&$db, &$objId, &$format, &$objectInDB){
		if(isset($objectInDB['sys_version'])) {
			return false;
		}
		if(!isset($format->fields['noversion'])) {
			$objId = null;
		}
		return true;
	}

	function OnBeforeModifyObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues, &$objectInDB) {
		if(isset($objectInDB['sys_version'])) {
			return false;
		}

		if(!isset($format->fields['noversion'])) {
			return PXRegistry::getApp()->plugins['contentversion']->popupAction();
		}
		return true;
	}

	function OnBeforeMoveObject(&$db, &$objectId, &$format, &$objectInDB){
		if(isset($objectInDB['sys_version'])) {
			return false;
		}
		return true;
	}
}
?>
