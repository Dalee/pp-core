<?php

/**
 * Helpers for testing
 */

define('TEST_CASE_NAME_LENGTH', ((int)($w = trim((string) `tput cols`)) ? $w : 80) - 36);

/**
 * unit test worker
 *
 * @version 0.1
 * @package commands
 * @see     PXCommandTest
 */
function t($name, $callback, $expected = true) {
	$verbose = PXCommandTest::$verbose;
	$diffmode = PXCommandTest::$diffmode;
	static $counter = 0;
	$counter++;

	$firstTime = microtime(1);
	$result = $callback();
	$elapsedTime = microtime(1) - $firstTime;

	_test_prepare($result);

	$test = is_callable($expected)
		? $expected($result)
		: json_encode($result) === json_encode($expected); // todo: make it more flexible

	$nameLength = TEST_CASE_NAME_LENGTH;
	$case = sprintf("%-{$nameLength}s", (mb_strlen((string) $name) > $nameLength ? mb_substr((string) $name, 0, $nameLength-1) . 'Б─╕' : $name));
	con_printf('%3d. %s  %+16s', $counter, $case, number_format($elapsedTime * 1000, 4, ',', ' ').'ms   ');
	con_printf(sprintf('[%s{end}]{nl}', $test ? '{fg:lgreen} OK ' : '{fg:lred}FAIL'));

	if ($verbose && !$test) {
		if ($diffmode) {
			$left = '/tmp/.tmp.radtest.expected';
			$right = '/tmp/.tmp.radtest.result';
			file_put_contents($left, _test_format($expected)."\n");
			file_put_contents($right, _test_format($result)."\n");
			if ($diffmode == 'sidebyside') {
				echo mb_str_pad('Expected:', 128/2, ' ') . mb_str_pad('Got:', 128/2, ' ') . PHP_EOL;
				echo `colordiff "$left" "$right" -y -i -d`;
			} else {
				echo `colordiff "$left" "$right" -u -i -d`;
			}
			unlink($left);
			unlink($right);
			echo PHP_EOL;
		} else {
			echo 'Expected:'.PHP_EOL;
			echo _test_format($expected);
			echo PHP_EOL;
			echo 'But got:'.PHP_EOL;
			echo _test_format($result);
			echo PHP_EOL;
		}
		if ($verbose > 1) {
			var_dump($result);
		}
	}
	$test || PXCommandTest::_fail(compact($counter, $name, $expected, $result));
}

/**
 * Output header
 */
function thead($class, $method = null) {
	static $counter = 0;
	is_null($method) && $method = '*';
	$letter = _tt_letter($counter++);
	con_printf("{fg:yellow}%2s.{end} {fg:white}%s.{fg:yellow}%s{end}:{nl}", $letter, $class, $method);
}

/**
 * Autogenerator for phpDoc tests
 *
 */
function tt ($file) {
	if (!file_exists($file)) {
		echo "Aim \"{$file}\" not found\n";
		return;
	}
	$toks = \PhpToken::tokenize(file_get_contents($file));
	// handle functions and classes
	$ns = null;
	$class = null;
	for ($i = 2, $l = count($toks); $i < $l; $i++) {
		if ($toks[$i-2][0] === T_NAMESPACE) {
			$ns = "\\";
			while ($toks[$i][0] !== T_WHITESPACE) {
				$ns .= $toks[$i][1];
				$i ++;
			}
			$ns .= "\\";
		} elseif ($toks[$i-2][0] === T_CLASS and $toks[$i][0] === T_STRING) {
			$class = $ns . $toks[$i][1];
			_tt_class($class);
		} elseif ($toks[$i-2][0] === T_FUNCTION and $toks[$i][0] === T_STRING) {
			$func = $ns . "\\" . $toks[$i][1];
			is_callable($func) && _tt_function($func);
		}
	}
}

/**
 * making roman letters by number
 * @param int $n
 * @param int $leftPad
 * @return string
 */
function _tt_letter($n, $leftPad = 0) {
	for ($r = ""; $n >= 0; $n = intval($n / 26) - 1) {
		$r = chr($n%26 + 0x61) . $r;
	}
	$leftPad && ($r = mb_str_pad($r, $leftPad, ' ', STR_PAD_LEFT));
	return $r;
}

function _tt_function($aim) {
	$method = new ReflectionFunction($aim);

	$tests = _parsePhpDocTest($method->getDocComment());
	if (empty($tests)) {
		return;
	}
	$methodname = "\\" . $method->getName();
	thead($method->getName());
	foreach ($tests as $k => $test) {
		$res = array_pop($test);
		$case = trim((string) $k);
		t($case, fn() => call_user_func_array($methodname, $test), $res);
	}
}

function _tt_class($aim) {
	$cref = new ReflectionClass($aim);
	$methods = $cref->getMethods(ReflectionMethod::IS_STATIC || ReflectionMethod::IS_PUBLIC);
	foreach ($methods as $method) {
		$tests = _parsePhpDocTest($method->getDocComment());
		if (empty($tests)) {
			continue;
		}
		$methodname = "\\" . $cref->getName().'::'.$method->getName();
		thead($cref->getName(), $method->getName());
		foreach ($tests as $k => $test) {
			$res = array_pop($test);
			$case = trim((string) $k);
			t($case, fn() => call_user_func_array($methodname, $test), $res);
		}
	}
}

function _parsePhpDocTest($phpdoc) {
	$tests = [];
	$pos = 0;
	while ($pos = mb_strpos((string) $phpdoc, '@test ', $pos)) {
		$end = mb_strpos((string) $phpdoc, "\n", $pos + 1);
		//$end = strpos($phpdoc, '@test', $pos + 1) ?: strpos($phpdoc, '@', $pos + 1) ?: strpos($phpdoc, '**/', $pos + 1);
		($end === false) && $end = mb_strlen((string) $phpdoc);

		$data = mb_substr((string) $phpdoc, $pos + 5, $end - $pos - 5);
		[$expression, $title] = array_pad(explode('---', $data), 2, null);
		[$params, $result] = array_pad(explode('>>>', (string) $expression), 2, 'null');
		$test = (array)json_decode(trim((string) $params), true);
		preg_match('@^/.+/[ism]?$@s', trim((string) $result), $m);
		$test[] = !$m?
			json_decode(trim((string) $result))
			: fn($expected) => preg_match($m[0], (string) $expected)? $expected : $m[0];
		$tests[$title ?: $data] = $test;
		$pos ++;
	}
	return $tests;
}

function _test_prepare(&$out) {
	if (is_object($out)) {
		if (method_exists($out, '__toString')) {
			$out = (string)$out;
		} elseif (method_exists($out, '__toArray')) {
			$out = $out->__toArray();
		} else {
			$out = (array)$out;
		}
	}
	if (is_array($out)) {
		array_walk_recursive($out, '_test_prepare');
	}
}

function _test_format($out) {
	$out = json_encode($out, JSON_PRETTY_PRINT);
	$out = preg_replace_callback('/(?:\\\\u[0-9a-fA-Z]{4})+/'.REGEX_MOD, function ($v) {
		$v = mb_strtr($v[0], ['\\u' => '']);
		return mb_convert_encoding(pack('H*', $v), CHARSET_UTF8, CHARSET_UTF16BE);
	}, $out);
	return $out;
}
