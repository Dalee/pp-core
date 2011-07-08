#!/usr/bin/php5
<?php
	/* let it be */
	define(BASEPATH, realpath(dirname(__FILE__) . '/../../../../')."/");

	require_once BASEPATH . '/libpp/lib/mainuser.inc';
	require_once BASEPATH . '/libpp/lib/mainadmin.inc';
	require_once BASEPATH . '/libpp/lib/maincommon.inc';

	#require_once BASEPATH . '/libpp/vendor/simpletest/autorun.php';
	require_once BASEPATH . '/libpp/vendor/simpletest/unit_tester.php';
	require_once BASEPATH . '/libpp/vendor/simpletest/reporter.php';
	require_once BASEPATH . '/libpp/vendor/simpletest/collector.php';
	require_once BASEPATH . '/libpp/vendor/simpletest/mock_objects.php';
	
	/* Please do includes in your own tests. */
	$unit_tests_dir = BASEPATH . '/libpp/plugins/multipleregions/tests/';

	$tests = new TestSuite('MultipleRegions plugin tests');

	$test_single_file = isset($_SERVER['argv'][1]) ? $_SERVER['argv'][1] : NULL;

	if ($test_single_file) {
		$tests->addFile(realpath($unit_tests_dir . '/' . $test_single_file));
	} else {
	    $tests->collect($unit_tests_dir, new RecursivePatternCollector("#_test.php$#i"));
	}

	$tests->run(new TextReporter());
?>
