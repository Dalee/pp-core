<?php

/**
 * path_resolve ([$from, ...], $to)
 * Resolves $to to an absolute path.
 * @param string $from,... unlimited number of paths. cwd by default
 * @param string $to resolving path
 * @return string resolved path
 * @test [".", "./foo"] >>> /.+\/foo$/
 * @test ["/foo/./bar/","./baz"] >>> "/foo/bar/baz"
 * @test ["/home/web","./foo"] >>> "/home/web/foo"
 * @test ["/home/web","../foo"] >>> "/home/foo"
 * @test ["/home/web/","../foo"] >>> "/home/foo"
 * @test ["/web/","../foo/"] >>> "/foo/"
 * @test ["yolo/swag/../yep","../nope","../foo"] >>> /.*\/yolo\/foo$/
 * @test ["foo/bar", "/tmp/file/", "..", "a/../subfile"] >>> "/tmp/subfile"
 * @test ["/foo/bar", "./baz"] >>> "/foo/bar/baz"
 */
function path_resolve ($from, $to = null) {
	$res = array();

	$paths = func_get_args();
	if (!path_is_absolute($paths[0])) {
		array_unshift($paths, IS_CLI? getcwd() : BASEPATH);
	}

	foreach ($paths as $path) {
		$parts = explode('/', rtrim($path, '/'));
		if (path_is_absolute($path)) {
			$res = array();
		}
		foreach ($parts as $part) {
			if (empty($part) || $part === '.') {
				continue;
			}
			elseif ($part === '..') {
				array_pop($res);
			}
			else {
				array_push($res, $part);
			}
		}
	}

	$lastPath = end($paths);
	$lastSlash = $lastPath[strlen($lastPath)-1] === '/';

	array_unshift($res, '');
	$lastSlash && array_push($res, '');

	return join('/', $res);
}

/**
 * path_relative(from, to)
 * Solve the relative path from $from to $to.
 * @param string $from
 * @param string $to
 * @return string
 * @todo test ["C:\\orandea\\test\\aaa", "C:\\orandea\\impl\\bbb"] >>> "..\\..\\impl\\bbb"
 * @test ["/data/orandea/test/aaa", "/data/orandea/impl/bbb"] >>> "../../impl/bbb"
 * @test ["f/g", "h/i"] >>> "../../h/i"
 */
function path_relative ($from, $to) {
	$from = substr(path_resolve($from), 1);
	$to = substr(path_resolve($to), 1);

    $fromParts = explode('/', trim($from, '/'));
    $toParts = explode('/', trim($to, '/'));

    $length = min(sizeof($fromParts), sizeof($toParts));
    $samePartsLength = $length;
    for ($i = 0; $i < $length; $i++) {
      if ($fromParts[$i] !== $toParts[$i]) {
        $samePartsLength = $i;
        break;
      }
    }

    $outputParts = array();
    for ($i = $samePartsLength; $i < sizeof($fromParts); $i++) {
      array_push($outputParts, '..');
    }

    $outputParts = array_merge($outputParts, array_slice($toParts, $samePartsLength));

    return join('/', $outputParts);
}

/**
 * @param string $path
 * @return bool
 * @test ["/home"] >>> true
 * @test ["rel"] >>> false
 * @test ["./rel"] >>> false
 * @test ["."] >>> false
 */
function path_is_absolute ($path) {
	return $path[0] === '/';
}
