<?php
define('BACKTRACE', function_exists('debug_backtrace'));

function PXTrace() {
	if (!BACKTRACE) {
		return false;
	}

	$stack = debug_backtrace();
	array_shift($stack);

	$trace = [];

	foreach ($stack as $i => $call) {
		$e = [];

		$e['file'] = isset($call['file']) ? str_replace(BASEPATH, '', $call['file']) : NULL;
		$e['line'] = isset($call['file']) ? $call['line'] : NULL;

		$e['function'] = isset($call['class']) ? $call['class'].$call['type'] : '';
		$e['function'] .= $call['function'];

		$e['args'] = [];

		if(isset($call['args'])) {
			foreach ($call['args'] as $k=>$v) {
				$type = gettype($v);

				$value = match ($type) {
					'resource' => get_resource_type($v),
					'array' => '['.sizeof($v).']',
					'object' => $v::class,
					'boolean' => $v ? 'true' : 'false',
					'string' => "'$v'",
					'NULL' => 'NULL',
					default => (string)$v,
				};

				$e['args'][$k] = ['type'=>$type, 'value'=>$value];
			}

		}

		$trace[] = $e;

	}

	return $trace;
}
