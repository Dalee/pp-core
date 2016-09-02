<?php

define('NOT_CLI', isset($_SERVER['REQUEST_METHOD']));
define('CLI',    !isset($_SERVER['REQUEST_METHOD']));

require_once PPLIBPATH . 'Debug/trace.inc';
require_once PPLIBPATH . 'Debug/ErrorReporter/abstract.class.inc';
require_once PPLIBPATH . 'Debug/sbin.inc';

function getMicroTime() {
	list($usec, $sec) = explode(" ", microtime());
	return ((float)$usec + (float)$sec);
}
