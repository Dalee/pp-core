#!/usr/local/bin/php -q
<?
set_time_limit(0);
require_once dirname(__FILE__).'/../lib/maincommon.inc';

$engine = new PXEngineSbin();
$engine->init();
$app =& $engine->app;
if (!isset($app->modules['cronrun'])) return;

$cronModule = $app->modules['cronrun']->getModule();
$jobName    = isset($_SERVER['argv'][1]) ? $_SERVER['argv'][1] : NULL;

if (isset($cronModule->jobs[$jobName])) {
	ini_set('display_errors', 1);
	Label('Run: '.$jobName);

	$cronModule->RunJob($cronModule->jobs[$jobName], $app, time());

	echo "\n\n/* Press Enter button */\n";
	ListenInput();

} else {
	ini_set('display_errors', 0);
	Label('Run all jobs, if it\'s time');

	$cronModule->RunTasks($app, time());
}

?>
