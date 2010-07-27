#!/usr/bin/php5 -q
<?
set_time_limit(0);
ini_set('memory_limit','512M'); //for greedy scripts
require_once dirname(__FILE__).'/../../../lib/maincommon.inc';

$engine = new PXEngineSbin();
$db  = PXRegistry::getDB();

$r = $db->query("SELECT table_name FROM information_schema.columns WHERE column_name = 'sys_regions'");

if(empty($r)) {
	return;
}

$r = array_map("pos", $r);

foreach($r as $i) {
	$db->transactionBegin();
	Label($i);
	$db->modifyingQuery(sprintf("ALTER TABLE %s ADD sys_reflex_id INT, ADD deny_region_edit BOOL", $i));
	$db->transactionCommit();
}

?>
