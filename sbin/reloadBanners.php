#!/usr/bin/php5 -q
<?php
	set_time_limit(0);
	require_once dirname(__FILE__).'/../../libpp/lib/maincommon.inc';

	ini_set('display_errors', '1'); 

	$engine = new PXEngineSbin();

	$db = PXRegistry::getDB();

	$status = in_array('--null', $argv) ? null
		: (in_array('--false', $argv) ? false
		: true); // by default
	$onlymobile = in_array('--only-mobile', $argv);
	$banners = $db->getObjects($db->types['adbanner'], $status);

	Label('Processing '.($onlymobile?'mobile ':'').'banners with '.(is_null($status)?' any status':'status '.($status?'true':'false')).'.');
 	foreach ($banners as $b) {
		WorkProgress(false, count($banners));

		if (!$onlymobile || (isset($b['mobile_status']) && $b['mobile_status'])) {
			$db->modifyContentObject($db->types['adbanner'], $b);
		}
	}

	WorkProgress(true);
	Label('Done');
?>
