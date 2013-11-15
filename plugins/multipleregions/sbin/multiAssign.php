#!/usr/bin/php5 -q
<?php
	/*
		Скрипт для автоматизации добавления нового региона во все региональные объекты, опубликованные в указанном регионе.
	*/ 
	
	if (count($_SERVER['argv']) < 3) {
		printf("Usage: %s NEW_REGION_ID EXISTING_REGION_ID\n\n", basename(__FILE__));
		exit();
	}
	
	require_once dirname(__FILE__).'/../../../lib/mainuser.inc';

	Label('Start...');

	$engine = new PXEngineSbin();

	$db       = PXRegistry::getDb();
	$app      = PXRegistry::getApp();
	$new      = (int) $_SERVER['argv'][1];
	$existing = (int) $_SERVER['argv'][2];

	if (count($db->getObjectsByIdArray($app->types['sys_regions'], true, array($new, $existing), DB_SELECT_TABLE)) < 2) {
		FatalError("Some of given regions are not exists");
	}

	foreach($app->types as $type) {
		if(!isset($type->fields['sys_regions'])) continue;
		Label("Updating {$type->id} datatype ...");
		$count = $db->modifyingQuery(
			sprintf("UPDATE %s SET sys_regions = array_cat(sys_regions, '{%d}') WHERE %s", 
				$type->id, $new, $db->inArray('sys_regions', $existing)), 
			null, null, false, true);
		Label("\tDone. {$count} objects were updated");
	}

	Label('Fin.');
?>
