#!/usr/local/bin/php -q
<?php
	define('BASEPATH', realpath(dirname(__FILE__).'/../../../').'/');
	require_once BASEPATH.'/libpp/lib/maincommon.inc';

	$engine = new PXEngineSbin();
	$engine->init();

	Label('Start');

	$settings = $engine->db->getObjects($engine->app->types['setting'], null);
	foreach($settings as $s) {
		WorkProgress(false, count($settings), 1000);
		$engine->db->getObjectById($engine->app->types['setting'], $s['id']);
	}
	WorkProgress(true);

	Label('Create '.((count($settings) + 1) * 2).' files in cache');
?>
