<?php
define('NOT_CLI', isset($_SERVER['REQUEST_METHOD']));
define('CLI',    !isset($_SERVER['REQUEST_METHOD']));

require_once 'ErrorReporter/abstract.class.inc';
require_once 'profiler.classes.inc';
require_once 'sbin.inc';
require_once 'trace.inc';

function getMicroTime() {
	list($usec, $sec) = explode(" ", microtime());
	return ((float)$usec + (float)$sec);
}

function D($variable) { // H - HTML, T - text version
	echo "\n";

	if (NOT_CLI) {
		echo '<pre class="debug">';
	}

	switch(true) {
		case is_bool($variable):
			echo ($variable) ? 'TRUE' : 'FALSE';
			break;

		case is_object($variable):
		case is_array($variable):
			var_dump($variable);
			break;

		case is_null($variable):
			echo 'Variable IS NULL';
			break;

		default:
			echo 'string('.strlen($variable).')'.$variable;
			break;
	}

	if (NOT_CLI) {
		echo '</pre>';
	}

	echo "\n";
}

function d2_type2string($v) {
	switch(gettype($v)) {
		case 'resource':
			return 'Resource '.get_resource_type($v);
			break;

		case 'array':
			return 'Array '.sizeof($v);
			break;

		case 'object':
			$d = 'Object '.get_class($v);
			while(($v = get_parent_class($v)) !== false) {
				$d .= ' extend '.$v;
			}
			return $d;
			break;

		case 'boolean':
			return $v ? 'TRUE' : 'FALSE';
			break;

		case 'string':
			return "'$v'";
			break;

		case 'NULL':
			return 'NULL';
			break;

		default:
			return (string)$v;
			break;
	}
}

function d2_shortArray($variable) {
	$showAble = array();

	foreach($variable as $k=>$v) {
		$showAble[$k] = d2_type2string($v);
	}

	return $showAble;
}

function d2_shortObject($variable) {

	$showAble = new stdClass;

	$methods = array();
	foreach(get_class_methods($variable) as $k=>$v) {
		$methods[] = $v;
	}
	$showAble->_methods = $methods;

	foreach(get_object_vars($variable) as $k=>$v) {
		$showAble->$k = d2_type2string($v);
	}

	return $showAble;
}

function d2_encode($dump) {
	if(NOT_CLI) {
		$dump = htmlentities($dump, ENT_NOQUOTES, 'KOI8-R');
	}

	if(substr(PHP_OS, 0, 3) == 'WIN') {
		$enc = NOT_CLI ? 'w' : 'd';
		$dump = myconv('k', $enc, $dump);
	}

	return $dump;
}

function D2($variable, $full=true) {
	$dump = d3($variable, $full);

	if(NOT_CLI) {
		echo "\n".'<pre class="debug">'.$dump.'</pre>';
	} else {
		echo "\n".$dump."\n";
	}
}

function D3($variable, $full=true) {
	switch(true) {
		case !$full && is_array($variable):
			$variable = d2_shortArray($variable);
			break;

		case !$full && is_object($variable):
			$variable = d2_shortObject($variable);
			break;
	}

	if(is_null($variable) || is_bool($variable)) {
		$variable = d2_type2string($variable);
	}

	$dump = print_r($variable, true);
	$dump = d2_encode($dump);

	return $dump;
}
?>
