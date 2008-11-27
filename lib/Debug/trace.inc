<?php
define('BACKTRACE', function_exists('debug_backtrace'));

function PXTrace() {
	if (!BACKTRACE) {
		return false;
	}

	$stack = debug_backtrace();
	array_shift($stack);

	$trace = array();

	foreach ($stack as $i => $call) {
		$e = array();

		$e['file'] = isset($call['file']) ? str_replace(BASEPATH, '', $call['file']) : NULL;
		$e['line'] = isset($call['file']) ? $call['line'] : NULL;

		$e['function'] = isset($call['class']) ? $call['class'].$call['type'] : '';
		$e['function'] .= $call['function'];

		$e['args'] = array();

		if(isset($call['args'])) {
			foreach ($call['args'] as $k=>$v) {
				$type = gettype($v);

				switch($type) {
					case 'resource':
						$value = get_resource_type($v);
						break;

					case 'array':
						$value = '['.sizeof($v).']';
						break;

					case 'object':
						$value = get_class($v);
						break;

					case 'boolean':
						$value = $v ? 'true' : 'false';
						break;

					case 'string':
						$value = "'$v'";
						break;

					case 'NULL':
						$value = 'NULL';
						break;

					default:
						$value = (string)$v;
						break;
				}

				$e['args'][$k] = array('type'=>$type, 'value'=>$value);
			}

		}

		$trace[] = $e;

	}

	return $trace;
}
?>