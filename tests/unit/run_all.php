#!/usr/local/bin/php
<?php
	/* let it be */
	define(BASEPATH, realpath(dirname(__FILE__) . '/../../../')."/");
	
	require_once BASEPATH . '/libpp/vendor/simpletest/unit_tester.php';
	require_once BASEPATH . '/libpp/vendor/simpletest/reporter.php';
	require_once BASEPATH . '/libpp/vendor/simpletest/collector.php';
	require_once BASEPATH . '/libpp/vendor/simpletest/mock_objects.php';
	
	require_once BASEPATH . '/libpp/tests/unit/DisplayType/classes.php';
	require_once BASEPATH . '/libpp/tests/unit/StorageType/classes.php';

	/* Please do includes in your own tests. */
	$current_dir = BASEPATH . '/libpp/tests/unit';

	$tests =& new TestSuite('libpp tests');
	$tests->collect($current_dir, new RecursivePatternCollector('/_test.php$/i'));
	$tests->run(new TextReporter());
?>
