#!/usr/bin/php5 -q
<?php
	define('BASEPATH', realpath(dirname(__FILE__).'/../../../').'/');
	require_once BASEPATH.'/libpp/lib/maincommon.inc';

	ini_set('display_errors', 1);

	Label('Some string');

	require_once 'code_with_errors.inc';

	Label('And another string');
?>