#!/usr/bin/php5 -q
<?

if(!defined("STDIN")) {
	define("STDIN", fopen('php://stdin','r'));
}

error_reporting( E_ALL );

set_time_limit(0);
ini_set('memory_limit','512M'); //for greedy scripts
require_once dirname(__FILE__).'/../../../lib/maincommon.inc';

set_error_handler("errorH");

Label("Cleanup Regions. Delete null, 0, -1 values... Start");

function question() {
	Label("Warning. null region field will be replace on {}. Check your models. Delete not used sys_regions fields.");
	Label("Continue?[y/n]");
	
	$answer = trim(fread(STDIN, 80));

	if(!in_array($answer, array("y", "n"))) {
		question();
	} elseif($answer === 'n') {
		Label("Aborted");
		exit;
	}
}

question();

$engine = new PXEngineSbin();
$db  = PXRegistry::getDB();

$regionsField = PXMultiRegions::REGION_MARK;

$r = $db->query("SELECT table_name from information_schema.columns  where column_name = '{$regionsField}'");

if(empty($r)) {
	return;
}

function errorH() {
	return true;
}

$r = array_map("pos", $r);

function is_natural($number) {
	return $number > 0;
}

foreach($r as $i) {
	Label($i);

	$params = array(
		"pattern"      => 'select id, %2$s from %1$s',
		"table"        => $i,
		"regions_mark" => $regionsField
	);

	$temp = $db->query(call_user_func_array("sprintf", $params));


	if(empty($temp)) continue;

	foreach($temp as $k) {
		$regions = 
			array_filter(PXMultipleRegionsHelper::toArray($k[$regionsField]), "is_natural");

		$params["pattern"]           = "update %s set %s = '%s' where id = '%s'";
		$params["region_mark_value"] = PXMultipleRegionsHelper::toString($regions);
		$params['cond_value']        = $k['id'];
		
		$db->modifyingQuery(call_user_func_array("sprintf", $params));
	}
}

Label("Done");

