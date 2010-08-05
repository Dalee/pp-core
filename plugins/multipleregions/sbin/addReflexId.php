#!/usr/bin/php5 -q
<?
set_time_limit(0);
ini_set('memory_limit','512M'); //for greedy scripts
require_once dirname(__FILE__).'/../../../lib/maincommon.inc';

set_error_handler("errorH");

Label("Add reflex_id, deny_region_edit in database... Start");

$engine = new PXEngineSbin();
$db  = PXRegistry::getDB();

$r = $db->query("SELECT table_name FROM information_schema.columns WHERE column_name = 'sys_regions'");

if(empty($r)) {
	return;
}

function errorH() {
	$_ = func_get_args();

	
	if(strstr($_[1], "already")) {
		Label("Already exist");
	}

	return true;
}

$r = array_map("pos", $r);

foreach($r as $i) {
	Label($i);
	Label();

	$params = array(
		"pattern" => "alter table %s add %s %s",
		"table"   => $i,
		"field"   => PXMultipleRegionsReflexer::REFLEX_FIELD,
		"type"    => "int"
	);

	Label("Reflex field add");
	$db->modifyingQuery(call_user_func_array("sprintf", $params));

	Label("Deny edit region field add");
	$db->modifyingQuery(call_user_func_array("sprintf", 
		$params + array("field" => PXPublicRegionObjectCloner::DENY_EDIT_FIELD, "type" => "bool")));

	Label();
}

Label("Done");

?>
