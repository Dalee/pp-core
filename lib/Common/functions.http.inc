<?php
// pecl_http package emulation

function http_parse_params_array ($params) {
	$result = http_parse_params($params, HTTP_PARAMS_ALLOW_COMMA | HTTP_PARAMS_ALLOW_FAILURE);
	return $result->params;
}

if (!defined('HTTP_PARAMS_ALLOW_COMMA')) :
	define('HTTP_PARAMS_ALLOW_COMMA',   1);
	define('HTTP_PARAMS_ALLOW_FAILURE', 2);
	define('HTTP_PARAMS_RAISE_ERROR',   4);
	define('HTTP_PARAMS_DEFAULT',       7);
endif;

if (!is_callable('http_parse_params')) :

function http_parse_params_split ($string, $delim, $quote = '"') {
	$r = array();
	$i = 0;

	$prevpart = '';
	$token = $delim;
	$part = strtok($string, $token);
	do {
		if (strpos($part, $quote) !== false && strrpos($part, $quote) === strpos($part, $quote)) {
			$prevpart = $part.$token;
			$part = strtok($quote) . $quote;
			$r[] = $prevpart . $part;

			$prevpart = '';
			$part = strtok($token);
			if(!$part) break;
		}

		$r[] = $prevpart . $part;
		$prevpart = '';
		$i++;
	} while ($part = strtok($token));

	return $r;
}

function http_parse_params_dequote ($string, $quote = '"') {
	if (mb_substr($string, 0, 1) == $quote && mb_substr($string, -1, 1) == $quote && mb_substr($string, -2, 1) != '\\') {
		$string = mb_substr($string, 1, -1);
	}
	return $string;
}

function http_parse_params ($params) {
	$result = array();
	$preresult = array();

	$params = str_replace("\\\"", "\0\0", $params);

	foreach (http_parse_params_split($params, ';') as $p0) {
		foreach (http_parse_params_split($p0, ',') as $p1) {
			foreach (http_parse_params_split($p1, ' ') as $param) {
				$pos = mb_strpos($param, '=');
				if ($pos === false) {
					$result[] = str_replace("\0\0", "\"", http_parse_params_dequote(trim($param)));
				}
				else {
					$result[][trim(mb_substr($param,0,$pos))] = str_replace("\0\0", "\"", http_parse_params_dequote(trim(mb_substr($param,$pos+1))));
				}
			}
		}
	}

	$result_object = new stdClass();
	$result_object->params = $result;

	return $result_object;
}

endif;


/**
 * URL constants as defined in the PHP Manual under "Constants usable with
 * http_build_url()".
 *
 * @see http://us2.php.net/manual/en/http.constants.php#http.constants.url
 */
if (!defined('HTTP_URL_REPLACE')) :
	define('HTTP_URL_REPLACE',        0x001); // replace every part of the first URL when there's one of the second URL
	define('HTTP_URL_JOIN_PATH',      0x002); // join relative paths
	define('HTTP_URL_JOIN_QUERY',     0x004); // join query strings
	define('HTTP_URL_STRIP_USER',     0x008); // strip any user authentication information
	define('HTTP_URL_STRIP_PASS',     0x010); // strip any password authentication information
	define('HTTP_URL_STRIP_PORT',     0x020); // strip explicit port numbers
	define('HTTP_URL_STRIP_PATH',     0x040); // strip complete path
	define('HTTP_URL_STRIP_QUERY',    0x080); // strip query string
	define('HTTP_URL_STRIP_FRAGMENT', 0x100); // strip any fragments (#identifier)

	// HTTP_URL_STRIP_ALL and HTTP_URL_STRIP_AUTH cover several other flags.

	// strip any authentication information
	define('HTTP_URL_STRIP_AUTH',     0x018); // USER | PASS
	// strip anything but scheme and host
	define('HTTP_URL_STRIP_ALL',      0x1f8); // USER | PASS | PORT | PATH | QUERY | FRAGMENT
endif;

if (!function_exists('http_build_url')) :

/**
 * Build a URL.
 * The parts of the second URL will be merged into the first according to the flags argument.
 * @param mixed $url (part(s) of) an URL in form of a string or associative array like parse_url() returns
 * @param mixed $params same as the first argument
 * @param int $flags a bitmask of binary or'ed HTTP_URL constants; HTTP_URL_REPLACE is the default
 * @param array $newUrl if set, it will be filled with the parts of the composed url like parse_url() would return
 * @return string Returns the new URL as string on success or FALSE on failure.
 * @test ["//ya.ru/?q=123",{"scheme":"https","query":"q=yahoo"}] >>> "https://ya.ru/?q=yahoo"
 * @test ["http://mike@www.example.com/foo/bar", "./baz", 26] >>> "http://www.example.com/foo/baz"
 * @test ["http://mike:1234@www.example.com/foo/bar/", "./../baz", 18] >>> "http://mike@www.example.com/foo/baz"
 * @test ["/path/?query#anchor",{"scheme":"https","host":"ssl.example.com"}] >>> "https://ssl.example.com/path/?query#anchor"
 * @test ["/path/?query#anchor",{"scheme":"ftp","host":"ftp.example.com","port":21}] >>> "ftp://ftp.example.com:21/path/?query#anchor"
 * @test ["http://example.org/orig?q=1#f","https://www.example.com:9999/replaced#n",0] >>> "https://www.example.com:9999/replaced?q=1#n"
 * @test ["http://user@www.example.com/pub/index.php?a=b#files",{"scheme":"ftp","host":"ftp.example.com","path":"files/current/","query":"a=c"},286] >>> "ftp://ftp.example.com/pub/files/current/?a=c" --- replace scheme, host, path, query
 */
function http_build_url($url, $parts = array(), $flags = HTTP_URL_REPLACE, &$new_url = array()) {
	is_array($url) || $url = parse_url($url);
	is_array($parts) || $parts = parse_url($parts);

	isset($url['query']) && is_string($url['query']) || $url['query'] = null;
	isset($parts['query']) && is_string($parts['query']) || $parts['query'] = null;

	$keys = array('user', 'pass', 'port', 'path', 'query', 'fragment');

	// Schema and host are always replaced
	foreach (array('scheme', 'host') as $part) {
		if (isset($parts[$part])) {
			$url[$part] = $parts[$part];
		}
	}

	$flags = $flags?: HTTP_URL_REPLACE;

	// default behavior
	if ($flags & HTTP_URL_REPLACE) {
		foreach ($keys as $key) {
			if (isset($parts[$key])) {
				$url[$key] = $parts[$key];
			}
		}
	}
	// more concrete ruled by flags
	else {
		if (isset($parts['path']) && ($flags & HTTP_URL_JOIN_PATH)) {
			$lastSlash = substr($url['path'], -1, 1) === '/';
			// d20 //, $parts['path'], path_resolve($parts['path'], $url['path']));
			$url['path'] = path_resolve($url['path'], ($lastSlash? '' : '../') . $parts['path']);
		}

		if (isset($parts['query']) && ($flags & HTTP_URL_JOIN_QUERY)) {
			if (isset($url['query'])) {
				parse_str($url['query'], $url_query);
				parse_str($parts['query'], $parts_query);

				$url['query'] = http_build_query(
					array_replace_recursive(
						$url_query,
						$parts_query
					)
				);
			}
			else {
				$url['query'] = $parts['query'];
			}
		}
	}

	// stripper
	foreach ($keys as $key) {
		$strip = 'HTTP_URL_STRIP_' . strtoupper($key);
		if ($flags & constant($strip)) {
			unset($url[$key]);
		}
	}

	// compile
	$parsed_string = isset($url['scheme'])? $url['scheme'] . '://' : '';
	if (isset($url['user'])) {
		$parsed_string .= $url['user'] . (isset($url['pass'])? ':' . $url['pass'] : '') . '@';
	}
	$parsed_string .= isset($url['host'])? $url['host'] : '';
	$parsed_string .= isset($url['port'])? ':' . $url['port'] : '';
	$parsed_string .= empty($url['path'])? '/' : $url['path'];
	$parsed_string .= isset($url['query'])? '?' . $url['query'] : '';
	$parsed_string .= isset($url['fragment'])? '#' . $url['fragment'] : '';

	// set to the last param if passed
	$new_url = $url;

	return $parsed_string;
}

endif;
