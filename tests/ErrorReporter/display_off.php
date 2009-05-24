#!/usr/local/bin/php -q
<?php
	define('BASEPATH', realpath(dirname(__FILE__).'/../../../').'/');
	require_once BASEPATH.'/libpp/lib/maincommon.inc';

	ini_set('display_errors', 0);

	Label('Some string');

	require_once 'code_with_errors.inc';

	Label('And another string');
?>