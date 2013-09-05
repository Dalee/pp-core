#!/usr/bin/env php54
<?php

define('con_debug', array_key_exists('d', getopt('d')), 1);
$a = microtime(1);

// initialization
set_time_limit(0);
ini_set('memory_limit','512M'); // for greedy scripts
require_once dirname(__FILE__) . '/../lib/maincommon.inc';

error_reporting(error_reporting() ^ E_STRICT); // temporary hide away strict notices

require_once PPLIBPATH . 'Engine/command.class.inc';

if (file_exists($localLib = dirname(__FILE__) . '/../../local/lib/mainsbin.inc')) {
	include_once $localLib;
}

con_debug && con_debug("initialized for %.4fms\n", (microtime(1)-$a)*1000);

con_debug && PXProfiler::on();

$engine = new PXEngineCommand();
$engine->run(null, array_slice($argv, 1));

