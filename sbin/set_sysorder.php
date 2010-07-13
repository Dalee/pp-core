#!/usr/bin/php5 -q
<?php
	require_once dirname(__FILE__).'/../../libpp/lib/mainuser.inc';

	Label('Start...');
	
	$engine = new PXEngineSbin();

	$db  = PXRegistry::getDb();
	$app = PXRegistry::getApp();

	foreach ($app->types as $tableName => $type) {
		WorkProgress();
		
		@$db->query('UPDATE ' . $tableName . ' set sys_order = id WHERE sys_order IS NULL');
	}
	WorkProgress(true);

	Label('Done');
?>