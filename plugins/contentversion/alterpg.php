#!/usr/bin/php5 -q
<?php
require_once dirname(__FILE__).'/../../lib/maincommon.inc';

$engine = new PXEngineSbin();
$db = PXRegistry::getDB();

$tables = $db->query(<<<SQL
	SELECT tablename FROM pg_tables WHERE schemaname = 'public'
SQL
);

if(empty($tables)) {
	return;
}

foreach($tables as $table) {
	$db->query(<<<SQL
		ALTER TABLE {$table['tablename']} DROP sys_version;
		ALTER TABLE {$table['tablename']} DROP sys_original;
		ALTER TABLE {$table['tablename']} DROP sys_modifyer;

		ALTER TABLE {$table['tablename']} ADD sys_version  INT4;
		ALTER TABLE {$table['tablename']} ADD sys_original INT4;
		ALTER TABLE {$table['tablename']} ADD sys_modifyer INT4
SQL
	);
}


?>
