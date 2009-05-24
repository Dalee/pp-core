#!/usr/local/bin/php -q
<?php
	define('BASEPATH', realpath(dirname(__FILE__).'/../../../').'/');
	require_once BASEPATH.'/libpp/lib/maincommon.inc';

	Label('init');

	$engine = new PXEngineSbin();
	$engine->init();

	Label('Start cache clearing');
	$engine->db->db->cache->clear();
	Label('Done');
?>