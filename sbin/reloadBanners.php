#!/usr/bin/php5 -q
<?php
	set_time_limit(0);
	require_once dirname(__FILE__).'/../../libpp/lib/maincommon.inc';

	ini_set('display_errors', '1'); 

	$engine = new PXEngineSbin();

	if (in_array('--help', $argv) || in_array('-?', $argv) || in_array('-h', $argv)) {
		Label("Banners reloader");
		Label("  --null, --false, --true   for reload banners with that status");
		Label("  --only-mobile             for filter by mobile_status (if exist)");
		exit;
	}

	$db = PXRegistry::getDB();

	$status = in_array('--null', $argv) ? null : !in_array('--false', $argv);
	$onlymobile = in_array('--only-mobile', $argv);
	$banners = $db->getObjects($db->types['adbanner'], $status);

	Label('Processing '.($onlymobile?'mobile ':'').'banners with '.(is_null($status)?' any status':'status '.($status?'true':'false')).'.');
 	foreach ($banners as $b) {
		WorkProgress(false, count($banners));

		// filter by mobile_status
		if ($onlymobile && !(isset($b['mobile_status']) && $b['mobile_status'])) {
			continue;
		}

		$db->modifyContentObject($db->types['adbanner'], $b);
	}

	WorkProgress(true);
	Label('Done');
?>
