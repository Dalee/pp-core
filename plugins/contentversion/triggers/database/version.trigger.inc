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

	function OnBeforeGetDataBuildQuery($db, $format, &$mode, &$selector, &$where, &$order, &$limit) {
		$app    = PXRegistry::getApp();
		$plugin = $app->plugins['contentversion']->getPlugin($app);

		if(!isset($format->fields['noversion'])) {
			if($mode != DB_SELECT_COUNT) {
				$selector .= ', '.$format->id.'.sys_original, '.$format->id.'.sys_modifyer, '.$format->id.'.sys_version';
			}

			if(strpos($where, $format->id.'.id') === false && !$plugin->description->params['no_modify_query']) {
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
		
		if(!isset($format->fields['noversion'])) {
			$action = PXRegistry::getRequest()->getAction();
			return PXRegistry::getApp()->plugins['contentversion']->getPlugin(PXRegistry::getApp())->invoke($action, $format->id, $objectInDB);
		}
		
		return true;
	}

	function OnBeforeModifyObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues, &$objectInDB) {
		$version = @$object['i_am_version'];

		if($version) {
			return true;
		}

		if(isset($objectInDB['sys_version'])) {
			PXRegistry::getApp()->plugins['contentversion']->getPlugin(PXRegistry::getApp())->version = true;
			return false;
		}

		if(!isset($format->fields['noversion'])) {
			$action = PXRegistry::getRequest()->getAction();
			return PXRegistry::getApp()->plugins['contentversion']->getPlugin(PXRegistry::getApp())->invoke($action, $format->id, $objectInDB, $version);
		}

		return true;
	}

	function OnBeforeMoveObject(&$db, &$objectId, &$format, &$objectInDB){
		if(isset($objectInDB['sys_version'])) {
			return false;
		}
		return true;
	}

	function OnBeforeNormalizePathBuildQuery($format, $exObjectFormat, &$table, $object) {
		if(isset($object['sys_version'])) {
			$w = 'sys_version is not null and sys_version = -1';
		} else {
			$w = 'sys_version is null';
		}

		strlen($table['exWhere']) && $table['exWhere'] .= " and ";
		$table['exWhere'] .=  $w;
	}
}
?>
