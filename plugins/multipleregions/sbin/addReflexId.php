#!/usr/bin/php5 -q
<?
set_time_limit(0);
ini_set('memory_limit','512M'); //for greedy scripts
require_once dirname(__FILE__).'/../../../lib/maincommon.inc';

$engine = new PXEngineSbin();
$app = PXRegistry::getApp();
$db  = PXRegistry::getDB();

$r = $db->query("select table_name from information_schema.columns where column_name = 'sys_regions'");

if(empty($r)) return;

$r = array_map("pos", $r);

foreach($r as $i) {
	$db->transactionBegin();
	$db->modifyingQuery(sprintf("alter table %s add reflex_id int, add deny_region_edit bool", $i));
	$db->transactionCommit();
}

?>
