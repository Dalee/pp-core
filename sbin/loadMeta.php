#!/usr/bin/env php5
<?php
	set_time_limit(0);
	require_once dirname(__FILE__).'/../../libpp/lib/maincommon.inc';

	ini_set('display_errors', '1'); 
	$engine = new PXEngineSbin();

	$skipTypes = array (
		'sgroup',
		'suser',
	);

	// process every object in database and update sys_meta tag
	$app = PXRegistry::getApp();
	$db = PXRegistry::getDb();

	$limit = 50;
	foreach($app->types as $type) {
		if (in_array($type->id, $skipTypes)) {
			continue;
		}
		
		Label(sprintf("processing: %s", $type->id));
		$totalObjects = $db->getObjects($type, null, DB_SELECT_COUNT);
		$offset = 0;

		WorkProgress(false, $totalObjects, $limit);
		while ( $offset < $totalObjects ) {
			$objectList = $db->getObjectsLimited($type, null, $limit, $offset);
			foreach($objectList as $object) {
				$db->ModifyContentObject($type, $object);
			}
			$offset += $limit;
			WorkProgress();
		}
		WorkProgress(true);
		break;
	}
?>