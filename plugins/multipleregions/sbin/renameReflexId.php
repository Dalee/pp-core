#!/usr/bin/php5 -q
<?php

error_reporting( E_ALL );

set_time_limit(0);
ini_set('memory_limit','512M'); //for greedy scripts
require_once dirname(__FILE__).'/../../../lib/maincommon.inc';

set_error_handler("errorH");

Label("Rename reflex_id in database... Start");

$engine = new PXEngineSbin();
$engine->start();

$db  = PXRegistry::getDB();

$r = $db->query("SELECT table_name, column_name from information_schema.columns  where column_name in ('reflex_id', 'sys_reflex_id')");

if(empty($r)) {
	return;
}

function errorH() {
	$_ = func_get_args();

	if(strstr($_[1], "does not exist")) {
		Label("Does not exists.");
	}

	return true;
}

foreach($r as $i) {
	Label($i['table_name']);

	$params = [
		"pattern" => "alter table %s rename %s to %s",
		"table"   => $i['table_name'],
		"field"   => $i['column_name'],
		PXMultipleRegionsReflexer::REFLEX_FIELD
  ];

	$db->modifyingQuery(call_user_func_array("sprintf", $params));
}

Label("Done");

