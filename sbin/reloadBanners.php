#!/usr/bin/php5 -q
<?php
	set_time_limit(0);
	require_once dirname(__FILE__).'/../../libpp/lib/maincommon.inc';

	ini_set('display_errors', '1'); 

	$engine = new PXEngineSbin();

	if (in_array('--help', $argv) || in_array('-?', $argv) || in_array('-h', $argv)) {
		Label("Banners reloader");
		Label("  --all, --off, --on   for reload banners with that status. on by default");
		exit;
	}

	$db = PXRegistry::getDB();

	$status = in_array('--all', $argv) ? null : !in_array('--off', $argv);
	$banners = $db->getObjects($db->types['adbanner'], $status);

	Label('Processing banners with '.(is_null($status)?' any status':'status '.($status?'true':'false')).'.');
 	foreach ($banners as $b) {
		WorkProgress(false, count($banners));
		$db->modifyContentObject($db->types['adbanner'], $b);
	}

	WorkProgress(true);
	Label('Done');
?>
