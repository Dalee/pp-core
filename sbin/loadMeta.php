#!/usr/bin/env php5
<?php
	set_time_limit(0);
	require_once dirname(__FILE__).'/../../libpp/lib/maincommon.inc';
	$localcommon = BASEPATH.'local/lib/maincommon.inc';
	if (file_exists($localcommon)) {
		require_once($localcommon);
	}

	ini_set('display_errors', '1'); 
	$engine = new PXEngineSbin();

	// process every object in database and update sys_meta tag
	$app = PXRegistry::getApp();
	$db = PXRegistry::getDb();
	$limit = 100;

	foreach($app->types as $type) {
		// is we need to run on this type?
		$dummyObject = array();
		$needProcess = false;
		foreach ($type->fields as $k => $v) {
			if ($v->storageType->notInDb($v, $dummyObject)) {
				$needProcess = true;
				break;
			}
		}

		if(!$needProcess) {
			Label(sprintf("No need to be processed: %s", $type->id));
			continue;
		}
		
		Label(sprintf("Processing: %s", $type->id));
		$offset = 0;
		$queryFmt = 'UPDATE %s SET %s WHERE id = %s';

		while ( ($objectList = $db->getObjectsLimited($type, null, $limit, $offset)) ) {
			foreach($objectList as $object) {
				WorkProgress(false);

				$sysMetaField = array();
				foreach ($type->fields as $k => $v) {
					if ($v->storageType->notInDb($v, $object)) {
						$p = array('id' => $object['id'], 'format' => $type->id);
						if ( ($proceedFileResult = $v->storageType->proceedFile($v, $object, $p)) ) {
							$sysMetaField[$k] = $proceedFileResult;
						}
					}
				}

				$metaField = (count($sysMetaField) > 0) ? $db->MapData(json_encode_koi($sysMetaField)) : 'NULL';
				$metaField = sprintf("sys_meta = %s", $metaField);

				// fire!
				$query = sprintf($queryFmt, $type->id, $metaField, $object['id']);
				$db->query($query);
			}

			$offset += $limit;
		}
		WorkProgress(true);
	}
?>