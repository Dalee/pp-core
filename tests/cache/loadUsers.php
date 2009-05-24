#!/usr/local/bin/php -q
<?php
	define('BASEPATH', realpath(dirname(__FILE__).'/../../../').'/');
	require_once BASEPATH.'/libpp/lib/maincommon.inc';

	$engine = new PXEngineSbin();
	$engine->init();

	Label('Start');

	$ids = $engine->db->query('SELECT id FROM suser');
	foreach($ids as $id) {
		WorkProgress(false, count($ids), 50);
		$engine->db->getObjectById($engine->app->types['suser'], $id['id']);
	}
	WorkProgress(true);

	Label('Create '.((count($ids) + 1) * 2).' files in cache');
?>
