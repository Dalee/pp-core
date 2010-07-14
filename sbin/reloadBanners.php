#!/usr/bin/php5 -q
<?php
	set_time_limit(0);
	require_once dirname(__FILE__).'/../../libpp/lib/maincommon.inc';

	ini_set('display_errors', '1'); 

	$engine = new PXEngineSbin();

	$db = PXRegistry::getDB();

	$banners = $db->getObjects($db->types['adbanner'], null);

 	foreach($banners as $b) {
		WorkProgress(false, count($banners));

		if($b['status']) {
			$db->modifyContentObject($db->types['adbanner'], $b);
		}
	}

	WorkProgress(true);
	Label('Done');
?>
