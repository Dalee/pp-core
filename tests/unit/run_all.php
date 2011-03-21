#!/usr/bin/php5
<?php
	define(BASEPATH, realpath(dirname(__FILE__) . '/../../../')."/");

	require_once BASEPATH . '/libpp/lib/mainuser.inc';
	require_once BASEPATH . '/libpp/lib/mainadmin.inc';
	require_once BASEPATH . '/libpp/lib/maincommon.inc';
	
	// require_once BASEPATH . '/libpp/vendor/simpletest/autorun.php';
	require_once BASEPATH . '/libpp/vendor/simpletest/unit_tester.php';
	require_once BASEPATH . '/libpp/vendor/simpletest/reporter.php';
	require_once BASEPATH . '/libpp/vendor/simpletest/collector.php';
	require_once BASEPATH . '/libpp/vendor/simpletest/mock_objects.php';
	
	/* Please do includes in your own tests. */
	$unit_tests_dir = BASEPATH . '/libpp/tests/unit';

	$tests = new TestSuite('libpp tests');

	if (isset($_SERVER['argv'][1])) {
		$file = $_SERVER['argv'][1];
		if (strlen($file) && $file[0] != '/') {
			$file = realpath($unit_tests_dir . '/' . $file);
		}
		$tests->addFile($file);
	} else {
		$tests->collect($unit_tests_dir, new RecursivePatternCollector("/_test.php$/i"));
	}

	$tests->run(new TextReporter());
?>
