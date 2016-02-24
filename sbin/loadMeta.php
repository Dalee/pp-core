#!/usr/bin/env php54
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
		$needProcess = false;
		foreach ($type->fields as $v) {
			if (!$v->storageType->storedInDb()) {
				$needProcess = true;
				break;
			}
		}

		if(!$needProcess) {
			Label(sprintf("No need to be processed: %s", $type->id));
			continue;
		}

		Label(sprintf("Processing: %s", $type->id));
		$queryUpdateFmt = 'UPDATE %s SET %s WHERE id = %s';
		$querySelectFmt = 'SELECT * FROM %s WHERE id > %d ORDER BY id ASC LIMIT %d';
		$lastId = 0;


		while ( true ) {
			$selector = sprintf($querySelectFmt, $type->id, $lastId, $limit);
			$objectList = $db->Query($selector);
			if (empty($objectList)) {
				break;
			}

			$db->_NormalizeTable($objectList, $type, false);
			foreach($objectList as $object) {
				WorkProgress(false);

				$sysMetaField = array();
				foreach ($type->fields as $k => $v) {
					if (!$v->storageType->storedInDb()) {
						$p = array('id' => $object['id'], 'format' => $type->id);
						if ( ($proceedFileResult = $v->storageType->proceedFile($v, $object, $p)) ) {
							$sysMetaField[$k] = $proceedFileResult;
						}
					}
				}

				$metaField = (count($sysMetaField) > 0) ? $db->MapData(json_encode($sysMetaField)) : 'NULL';
				$metaField = sprintf("sys_meta = %s", $metaField);

				// fire!
				$lastId = $object['id'];
				$query = sprintf($queryUpdateFmt, $type->id, $metaField, $lastId);
				$db->query($query);
			}
		}
		WorkProgress(true);
	}
