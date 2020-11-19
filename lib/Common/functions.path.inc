<?php

/**
 * @param $path
 * @return string
 *
 * @test ["/hello//world"] >>> "/hello/world"
 * @test ["hello//world"] >>> "hello/world"
 * @test ["../etc/passwd"] >>> "etc/passwd"
 * @test ["/etc/./passwd/?=hello"] >>> "/etc/passwd/?=hello"
 * @test [""] >>> ""
 */
function path_clear($path) {
	$pathItems = explode(DIRECTORY_SEPARATOR, $path);

	$pathItems = array_map(function ($item) {
		return path_filter_filename((string)$item);
	}, $pathItems);

	$pathItems = array_filter($pathItems);

	$result = implode(DIRECTORY_SEPARATOR, $pathItems);
	if (path_is_absolute($path)) {
		$result = DIRECTORY_SEPARATOR . $result;
	}

	return $result;
}

/**
 * https://stackoverflow.com/questions/2021624/string-sanitizer-for-filename#42058764
 * @param string $filename
 * @return string
 */
function path_filter_filename($filename) {
	// sanitize filename
	$filename = preg_replace(
		'~
        [<>:"/\\\|?*]|           # file system reserved https://en.wikipedia.org/wiki/Filename#Reserved_characters_and_words
        [\x00-\x1F]|             # control characters http://msdn.microsoft.com/en-us/library/windows/desktop/aa365247%28v=vs.85%29.aspx
        [\x7F\xA0\xAD]|          # non-printing characters DEL, NO-BREAK SPACE, SOFT HYPHEN
        [#\[\]@!$&\'()+,;=]|     # URI reserved https://tools.ietf.org/html/rfc3986#section-2.2
        [{}^\~`]                 # URL unsafe characters https://www.ietf.org/rfc/rfc1738.txt
        ~xu',
		'-', $filename);
	// avoids ".", ".." or ".hiddenFiles"
	$filename = ltrim($filename, '.-');
	$filename = preg_replace('/-+/', '-', $filename);
	// maximize filename length to 255 bytes http://serverfault.com/a/9548/44086
	$ext = pathinfo($filename, PATHINFO_EXTENSION);
	$filename = mb_strcut(pathinfo($filename, PATHINFO_FILENAME), 0,
			255 - ($ext ? strlen($ext) + 1 : 0), mb_detect_encoding($filename)) . ($ext ? '.' . $ext : '');
	return $filename;
}

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
function path_resolve($from, $to = null) {
	$res = [];

	$paths = func_get_args();
	if (!path_is_absolute($paths[0])) {
		array_unshift($paths, IS_CLI? getcwd() : BASEPATH);
	}

	foreach ($paths as $path) {
		$parts = explode('/', rtrim($path, '/'));
		if (path_is_absolute($path)) {
			$res = [];
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
function path_relative($from, $to) {
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

    $outputParts = [];
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
	return mb_substr($path, 0, 1) === '/';
}
