#!/usr/bin/env php5
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

if($jobName) {
	ini_set('display_errors', 1);

	if (isset($cronModule->jobs[$jobName])) {
		Label('Run: '.$jobName);

		$cronModule->runJob($cronModule->jobs[$jobName], $app, time());

	} else {
		Label('Choose job:');
		echo "\n";

		foreach($cronModule->jobs as $jobName => $j) {
			$stat = $cronModule->getStat($j);

			$_ = array(
				"\t",
				str_pad($jobName,             25),
				str_pad($j['rule']->asString, 25),
				str_pad($j['job']->name,      25),
				strftime('%Y-%m-%d %H:%M', $stat['start']),
				"\t",
				strftime('%Y-%m-%d %H:%M', $stat['end']),
				"\t",
				$stat['result']['note'],
				"\n"
			);

			echo implode('', $_);
		}
		
		echo "\n";
	}

} else {
	ini_set('display_errors', 0);
	Label('Run all jobs, if it\'s time');

	$cronModule->RunTasks($app, time());
}
?>