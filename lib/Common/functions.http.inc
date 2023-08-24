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
	$r = [];
	$i = 0;

	$prevpart = '';
	$token = $delim;
	$part = strtok($string, $token);
	do {
		if (str_contains($part, (string) $quote) && strrpos($part, (string) $quote) === strpos($part, (string) $quote)) {
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
	if (mb_substr((string) $string, 0, 1) == $quote && mb_substr((string) $string, -1, 1) == $quote && mb_substr((string) $string, -2, 1) != '\\') {
		$string = mb_substr((string) $string, 1, -1);
	}
	return $string;
}

function http_parse_params ($params) {
	$result = [];
	$preresult = [];

	$params = str_replace("\\\"", "\0\0", (string) $params);

	foreach (http_parse_params_split($params, ';') as $p0) {
		foreach (http_parse_params_split($p0, ',') as $p1) {
			foreach (http_parse_params_split($p1, ' ') as $param) {
				$pos = mb_strpos((string) $param, '=');
				if ($pos === false) {
					$result[] = str_replace("\0\0", "\"", (string) http_parse_params_dequote(trim((string) $param)));
				}
				else {
					$result[][trim(mb_substr((string) $param,0,$pos))] = str_replace("\0\0", "\"", (string) http_parse_params_dequote(trim(mb_substr((string) $param,$pos+1))));
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
 * @see http://git.php.net/?p=pecl/http/pecl_http.git;a=blob;f=php_http_url_api.h;h=940db8e61e68e1a544896a691079ad33c5b633ff;hb=375522dcffba5c1f2d99ff70afb2a8f8788063cd
 */
if (!defined('HTTP_URL_REPLACE')) :
	define('HTTP_URL_REPLACE',        0x000); // replace every part of the first URL when there's one of the second URL
	define('HTTP_URL_JOIN_PATH',      0x001); // join relative paths
	define('HTTP_URL_JOIN_QUERY',     0x002); // join query strings
	define('HTTP_URL_STRIP_USER',     0x004); // strip any user authentication information
	define('HTTP_URL_STRIP_PASS',     0x008); // strip any password authentication information
	define('HTTP_URL_STRIP_PORT',     0x020); // strip explicit port numbers
	define('HTTP_URL_STRIP_PATH',     0x040); // strip complete path
	define('HTTP_URL_STRIP_QUERY',    0x080); // strip query string
	define('HTTP_URL_STRIP_FRAGMENT', 0x100); // strip any fragments (#identifier)

	// HTTP_URL_STRIP_ALL and HTTP_URL_STRIP_AUTH cover several other flags.

	// strip any authentication information
	define('HTTP_URL_STRIP_AUTH',     HTTP_URL_STRIP_USER | HTTP_URL_STRIP_PASS); // 0x00C
	// strip anything but scheme and host
	define('HTTP_URL_STRIP_ALL',      HTTP_URL_STRIP_AUTH | HTTP_URL_STRIP_PORT | HTTP_URL_STRIP_PATH | HTTP_URL_STRIP_QUERY | HTTP_URL_STRIP_FRAGMENT);
	define('HTTP_URL_FROM_ENV',       0x1000);
endif;

if (!function_exists('http_build_url')) :

/**
 * Build a URL.
 *
 * The parts of the second URL will be merged into the first according to the flags argument.
 * @param mixed $url (part(s) of) an URL in form of a string or associative array like parse_url() returns
 * @param mixed $params same as the first argument
 * @param int $flags a bitmask of binary or'ed HTTP_URL constants; HTTP_URL_REPLACE is the default
 * @param array $newUrl if set, it will be filled with the parts of the composed url like parse_url() would return
 * @return string Returns the new URL as string on success or FALSE on failure.
 *
 * @see http://git.php.net/?p=pecl/http/pecl_http.git;a=blob;f=http_url_api.c;h=8a70b0f2fb18b3435110b436b7f1f68023cafb15;hb=refs/heads/R_1_7
 *
 * @test ["//ya.ru/?q=123",{"scheme":"https","query":"q=yahoo"}] >>> "https://ya.ru/?q=yahoo"
 *
 * @see http://git.php.net/?p=pecl/http/pecl_http.git;a=blob;f=tests/build_url_001.phpt;h=c8b5ad36c146e9868b358f4d518390d695905d69;hb=refs/heads/R_1_7
 * @test ["page", {}, 0] >>> "http://localhost/page"
 * @test ["with/some/path/", {}, 0] >>> "http://localhost/with/some/path/"
 *
 * @see http://git.php.net/?p=pecl/http/pecl_http.git;a=blob;f=tests/build_url_002.phpt;h=787b7f42840b22b5927f78328a19b45a9b0d7002;hb=refs/heads/R_1_7
 * @test ["http://example.org/orig?q=1#f", "https://www.example.com:9999/replaced#n"] >>> "https://www.example.com:9999/replaced?q=1#n"
 * @test ["http://example.org/orig?q=1#f", "https://www.example.com:9999/replaced#n", 0] >>> "https://www.example.com:9999/replaced?q=1#n"
 *
 * @see http://git.php.net/?p=pecl/http/pecl_http.git;a=blob;f=tests/build_url_003.phpt;h=1736ca6e3b45c77a0b683ea5765dcf25f836cb22;hb=refs/heads/R_1_7
 * @test ["/path/?query#anchor", {}, 0] >>> "http://localhost/path/?query#anchor"
 * @test ["/path/?query#anchor", {"scheme":"https"}, 0] >>> "https://localhost/path/?query#anchor"
 * @test ["/path/?query#anchor", {"scheme":"https", "host": "ssl.example.com" }, 0] >>> "https://ssl.example.com/path/?query#anchor"
 * @test ["/path/?query#anchor", {"scheme":"ftp", "host": "ftp.example.com", "port": 21 }, 0] >>> "ftp://ftp.example.com/path/?query#anchor"
 *
 * @see http://git.php.net/?p=pecl/http/pecl_http.git;a=blob;f=tests/build_url_004.phpt;h=1db54a8f1ae97faf08c52586adc2e7c32e2312c0;hb=refs/heads/R_1_7
 * @test ["http://mike@www.example.com/foo/bar", "./baz", 13] >>> "http://www.example.com/foo/baz" --- HTTP_URL_STRIP_AUTH|HTTP_URL_JOIN_PATH
 * @test ["http://mike@www.example.com/foo/bar/", "../baz", 5] >>> "http://www.example.com/foo/baz" --- HTTP_URL_STRIP_USER|HTTP_URL_JOIN_PATH
 * @test ["http://mike:1234@www.example.com/foo/bar/", "./../baz", 9] >>> "http://mike@www.example.com/foo/baz" --- HTTP_URL_STRIP_PASS|HTTP_URL_JOIN_PATH
 * @test ["http://www.example.com:8080/foo?a[0]=b#frag", "?a[0]=1&b=c&a[1]=b", 354] >>> "http://www.example.com/?a%5B0%5D=1&a%5B1%5D=b&b=c" --- HTTP_URL_JOIN_QUERY|HTTP_URL_STRIP_PORT|HTTP_URL_STRIP_FRAGMENT|HTTP_URL_STRIP_PATH
 *
 * @test ["http://example.org/orig?q=1#f","https://www.example.com:9999/replaced#n",0] >>> "https://www.example.com:9999/replaced?q=1#n"
 * @test ["http://user@www.example.com/pub/index.php?a=b#files",{"scheme":"ftp","host":"ftp.example.com","path":"files/current/","query":"a=c"},271] >>> "ftp://ftp.example.com/pub/files/current/?a=c" --- replace scheme, host, path, query == HTTP_URL_STRIP_FRAGMENT|HTTP_URL_STRIP_AUTH|HTTP_URL_JOIN_QUERY|HTTP_URL_JOIN_PATH
 */
function http_build_url(mixed $url, $parts = [], $flags = HTTP_URL_FROM_ENV, &$new_url = []) {
	is_array($url) || $url = parse_url((string) $url);
	is_array($parts) || $parts = parse_url((string) $parts);

	isset($url['query'])   && is_string($url['query'])   || $url['query']   = null;
	isset($parts['query']) && is_string($parts['query']) || $parts['query'] = null;

	$keys = ['user', 'pass', 'port', 'path', 'query', 'fragment'];

	$new_url = [];

	$new_url['port'] = 0;

	if(!($flags & HTTP_URL_STRIP_PORT)) {
		$new_url['port'] = $parts['port'] ?? $url['port'] ?? 0;
	}

	if(!($flags & HTTP_URL_STRIP_USER)) {
		$new_url['user'] = $parts['user'] ?? $url['user'] ?? null;
	}

	if(!($flags & HTTP_URL_STRIP_PASS)) {
		$new_url['pass'] = $parts['pass'] ?? $url['pass'] ?? null;
	}

	$new_url['scheme'] = $parts['scheme'] ?? $url['scheme'] ?? null;
	$new_url['host']   = $parts['host'] ?? $url['host'] ?? null;

	if(!($flags & HTTP_URL_STRIP_PATH)) {
		if(($flags & HTTP_URL_JOIN_PATH) && isset($url['path']) && isset($parts['path']) && !str_starts_with((string) $parts['path'], '/')){
			$new_url['path'] = str_ends_with((string) $url['path'], '/') ? $url['path'] : (dirname((string) $url['path']) . '/');
			$new_url['path'] .= $parts['path'];
		} else {
			$new_url['path'] = $parts['path'] ?? $url['path'] ?? null;
		}
	}

	if (!($flags & HTTP_URL_STRIP_QUERY)) {
		if(($flags & HTTP_URL_JOIN_QUERY) && isset($url['query']) && isset($parts['query'])) {
			parse_str((string) $url['query'], $url_query);
			parse_str((string) $parts['query'], $parts_query);
			$new_url['query'] = http_build_query(
				array_replace_recursive(
					$url_query,
					$parts_query
				)
			);
		} else {
			$new_url['query'] = $parts['query'] ?? $url['query'] ?? null;
		}
	}

	if(!($flags & HTTP_URL_STRIP_FRAGMENT)) {
		$new_url['fragment'] = $parts['fragment'] ?? $url['fragment'] ?? null;
	}

	if (!isset($new_url['scheme'])) {
		if ($flags & HTTP_URL_FROM_ENV) {
			$new_url['scheme'] = match (true) {
				$new_url['port'] == 443, !!$_SERVER['HTTPS'] && !strcasecmp((string) $_SERVER['HTTPS'], 'ON') => 'https',
				!!($scheme = getservbyport($new_url['port'], "tcp")) => $scheme,
				default => 'http',
			};
		} else {
			$new_url['scheme'] = 'http';
		}
	}

	if(!isset($new_url['host'])) {
		if($flags & HTTP_URL_FROM_ENV) {
			if(strlen((string) ($host = $_SERVER['HTTP_HOST'])) || strlen((string) ($host = $_SERVER['SERVER_NAME']))) {
				$new_url['host'] = $host;
			} else {
				$new_url['host'] = php_uname('n');
			}
		} else {
			$new_url['host'] = 'localhost';
		}
	}

	if(!isset($new_url['path'])) {
		if(($flags & HTTP_URL_FROM_ENV) && strlen((string) $_SERVER['REQUEST_URI'])) {
			$pos = strpos((string) $_SERVER['REQUEST_URI'], '?');
			if($pos !== false) {
				$new_url['path'] = substr((string) $_SERVER['REQUEST_URI'], 0, $pos);
			} else {
				$new_url['path'] = $_SERVER['REQUEST_URI'];
			}
		} else {
			$new_url['path'] = '/';
		}
	} elseif (!str_starts_with((string) $new_url['path'], '/')) {
		if(($flags & HTTP_URL_FROM_ENV) && strlen((string) $_SERVER['REQUEST_URI'])) {
			$pos = strrpos((string) $_SERVER['REQUEST_URI'], '/');
			if($pos !== false) {
				$path = substr((string) $_SERVER['REQUEST_URI'], 0, $pos);
			} else {
				$path = '/';
			}
		} else {
			$path = '/';
		}
		$new_url['path'] = $path . $new_url['path'];
	}

	if($new_url['path'] != '/'){
		while(preg_match('#/\.(/|$)#' . REGEX_MOD, (string) $new_url['path'], $matches, PREG_OFFSET_CAPTURE)) {
			$new_url['path'] = substr_replace((string) $new_url['path'], "/", $matches[0][1], strlen($matches[0][0]));
		}
		while(preg_match('#(^|/[^/]+)/\.\.(/|$)#' . REGEX_MOD, (string) $new_url['path'], $matches, PREG_OFFSET_CAPTURE)) {
			$new_url['path'] = substr_replace((string) $new_url['path'], "/", $matches[0][1], strlen($matches[0][0]));
		}
	}

	if($new_url['port']) {
		if(($new_url['port'] == 80 && $new_url['scheme'] == 'http') ||
		   ($new_url['port'] == 443 && $new_url['scheme'] == 'https') ||
		   (getservbyname($new_url['scheme'], 'tcp') == $new_url['port'])
		) {
			$new_url['port'] = 0;
		}
	}

	$str = $new_url['scheme'] . '://';
	if(isset($new_url['user'])) {
		$str .= $new_url['user'];
		$str .= isset($new_url['password']) ? ':' . $new_url['password'] : '';
		$str .= '@';
	}
	$str .= $new_url['host'];
	$str .= $new_url['port'] ? ':' . $new_url['port'] : '';
	$str .= $new_url['path'];
	$str .= isset($new_url['query']) ? '?' . $new_url['query'] : '';
	$str .= isset($new_url['fragment']) ? '#' . $new_url['fragment'] : '';

	return $str;
}

endif;
