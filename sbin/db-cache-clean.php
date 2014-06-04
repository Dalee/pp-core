#!/usr/bin/php5 -q
<?
set_time_limit(0);
ini_set('memory_limit','512M'); //for greedy scripts
require_once dirname(__FILE__).'/../lib/maincommon.inc';

if(file_exists($localLib = dirname(__FILE__).'/../../local/lib/mainsbin.inc')){
	include_once $localLib;
}

$engine = new PXEngineSbin();

$db = PXRegistry::getDb();
$db->clearCache(true);
