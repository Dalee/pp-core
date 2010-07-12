#!/usr/bin/php5 -q
<?php
	require_once dirname(__FILE__).'/../../libpp/lib/mainuser.inc';

	Label('Start...');
	
	$engine = new PXEngineSbin();

	$db  = PXRegistry::getDb();
	$app = PXRegistry::getApp();

	foreach ($app->types as $tableName => $type) {
		WorkProgress();
		
		$data = @$db->query("SELECT id, sys_order from ". $tableName);
	
		if (count($data)) {
			$orders = array();
			foreach ($data as $item) {
				if (!empty($item['sys_order'])) {
					$orders[] = $item['sys_order'];
				}
			}

			foreach ($data as $v) {
				if (empty($v['sys_order'])) {
					$sys_order = $v['id'];
					
					for ($i = $sys_order; in_array($i, $orders); $i++) {
						$sys_order = $i;
					}

					$db->query('UPDATE ' . $tableName . ' set sys_order = '.$sys_order.' where id = '.$v['id']);
				}
			}
		}
	}
	WorkProgress(true);

	Label('Done');
?>