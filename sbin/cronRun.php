#!/usr/bin/php5 -q
<?
set_time_limit(0);
ini_set('memory_limit','512M'); //for greedy scripts
require_once dirname(__FILE__).'/../lib/maincommon.inc';

if(file_exists($localLib = dirname(__FILE__).'/../../local/lib/mainsbin.inc')){
	include_once $localLib;
}

$engine = new PXEngineSbin();
$app = PXRegistry::getApp();

if (!isset($app->modules['cronrun'])) {
	return;
}

$cronModule = $app->modules['cronrun']->getModule();
$jobName    = isset($_SERVER['argv'][1]) ? $_SERVER['argv'][1] : NULL;

if (isset($cronModule->jobs[$jobName])) {
	ini_set('display_errors', 1);
	Label('Run: '.$jobName);

	$cronModule->runJob($cronModule->jobs[$jobName], $app, time());

} else {
	ini_set('display_errors', 0);
	Label('Choose job:');

	echo "\n";
	foreach($cronModule->jobs as $jobName => $j) {
		echo "\t".str_pad($jobName, 25).str_pad($j['rule']->asString, 25).$j['job']->name."\n";
	}
	echo "\n";

	Label('Run all jobs, if it\'s time');

	$cronModule->RunTasks($app, time());
}
?>