#!/usr/bin/php5 -q
<?php
	require_once dirname(__FILE__).'/../../../../libpp/lib/mainuser.inc';

	Label('Start...');

	$engine = new PXEngineSbin();

	$db  = PXRegistry::getDb();
	$app = PXRegistry::getApp();

	foreach(array('user', 'module') as $rt) {
		$counter = 1;

		foreach($db->query("select * from acl_objects where objectrule = '{$rt}' order by sys_order") as $a => $b) {
			$db->modifyingQuery(sprintf('update acl_objects set sys_order = %s where id = %s', $counter, $b['id']));
			$counter++;
		};
	}

	WorkProgress(true);
	Label('Done');
?>
