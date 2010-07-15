<?php
class PXTriggerDatabaseVersion extends PXAbstractDatabaseTrigger {
	function OnBeforeAddObject(&$db, &$newId, &$format, &$object, &$dbFields, &$dbValues) {
		$this->fillVersionFields($object, $dbFields, $dbValues);
		return true;
	}

	function OnBeforeGetDataBuildQuery($db, $format, &$mode, &$selector, &$where, &$order, &$limit) {
		$app    = PXRegistry::getApp();
		$plugin = $app->plugins['contentversion'];

		if(isset($format->fields['noversion'])) {
			return;
		}

		if($mode != DB_SELECT_COUNT) {
			$selector .= ', '.$format->id.'.sys_original, '.$format->id.'.sys_modifyer, '.$format->id.'.sys_version';
		}

		if(strpos($where, $format->id.'.id') === false && !$plugin->params['no_modify_query']) {
			$where = strlen($where) ? "$where AND sys_version IS NULL" : "sys_version IS NULL";
		}
	}

	function OnBeforeDeleteObject(&$db, &$objId, &$format, &$objectInDB) {
		if(isset($format->fields['noversion'])) {
			return true;
		}
		
		if(!isset($object['sys_version'])) {
			PXContentVersion::deleteOriginal($format, $objId);
		}

		return false;
	}

	function OnBeforeModifyObject(&$db, &$objId, &$format, &$object, &$dbFields, &$dbValues, &$objectInDB) {
		if(isset(PXRegistry::getApp()->plugins['contentversion']->params['skip_trigger'])) {
			return true;
		}

		if(isset($object['is_version'])) {
			$this->fillVersionFields($object, $dbFields, $dbValues);
			return true;
		}

		$version = !$this->isVersion($object) && !PXRegistry::getRequest()->getVar('version')
			? new PXContentVersion($format,   $object, $objectInDB)  # create new version
			: PXContentVersion::find($format, $object, $objectInDB); # back to version

		$version->save();

		return false;
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
	
	
	private function fillVersionFields($object, &$dbFields, &$dbValues) {
		if($this->isVersion($object)) {
			$dbFields[] = 'sys_version';
			$dbFields[] = 'sys_modifyer';
			$dbFields[] = 'sys_original';

			$dbValues[] = $object['sys_version'];
			$dbValues[] = $object['sys_modifyer'];
			$dbValues[] = $object['sys_original'];
		}
	}
	
	private function isVersion($object) {
		return isset($object['sys_version']);
	}
}
?>
