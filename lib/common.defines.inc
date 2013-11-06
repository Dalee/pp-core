<?php

// Default timezone settings for formated date output
date_default_timezone_set('Europe/Moscow');

// php5.4 htmlspecialchars flag ENT_HTML401
if (!defined('ENT_HTML401')) {
	define('ENT_HTML401', 0);
}

// PHP 5.2.6 compatibility
if (!defined('PHP_MAJOR_VERSION')) {
	define('PHP_MAJOR_VERSION', 5);
}
if (!defined('PHP_MINOR_VERSION')) {
	define('PHP_MINOR_VERSION', 2);
}
if (!defined('PHP_VERSION_ID')) {
	define('PHP_VERSION_ID', 50206);
}
if (!defined('PHP_RELEASE_VERSION')) {
	define('PHP_RELEASE_VERSION', 6); // PHP 5.2.6
}
if (!defined('PHP_EXTRA_VERSION')) {
	define('PHP_EXTRA_VERSION', '');
}

// Определение не на винде ли мы запущены
if (!defined('IS_WIN')) {
	define('IS_WIN', (substr(PHP_OS, 0, 3) == 'WIN'));
}

// Определение корня сайта в случае запуска скрипта из консоли (CLI)
if(!isset($_SERVER['REQUEST_METHOD'])) {
	$_SERVER['DOCUMENT_ROOT'] = realpath(dirname(__FILE__).'/../../');
}

// cli
defined('IS_CLI') || define('IS_CLI', PHP_SAPI === 'cli');

// Определение BASEPATH - это полный путь к корню сайта
if(!defined('BASEPATH')) {
	if (IS_WIN) {
		define('BASEPATH', rtrim(str_replace("\\\\", DIRECTORY_SEPARATOR, $_SERVER["DOCUMENT_ROOT"]), DIRECTORY_SEPARATOR) . DIRECTORY_SEPARATOR);
	} else {
		define('BASEPATH', rtrim(str_replace("/", DIRECTORY_SEPARATOR, $_SERVER["DOCUMENT_ROOT"]), DIRECTORY_SEPARATOR) . DIRECTORY_SEPARATOR);
	}
}

// Объявляем PPPATH, LOCALPATH, SHAREDPATH
defined('PPPATH')     || define('PPPATH',     BASEPATH . 'libpp' . DIRECTORY_SEPARATOR);
defined('PPLIBPATH')  || define('PPLIBPATH',  PPPATH   . 'lib' . DIRECTORY_SEPARATOR);
defined('LOCALPATH')  || define('LOCALPATH',  BASEPATH . 'local' . DIRECTORY_SEPARATOR);
defined('SHAREDPATH') || define('SHAREDPATH', BASEPATH . 'site' . DIRECTORY_SEPARATOR);

//
if (isset($_SERVER['HTTP_X_REWRITE_URL'])) {
	$tmp = @parse_url($_SERVER['HTTP_X_REWRITE_URL']);
	$_SERVER['SCRIPT_NAME'] = $tmp['path'];
	$_SERVER['REQUEST_URI'] = $_SERVER['HTTP_X_REWRITE_URL'];

	if (isset($tmp['query'])) {
		$_SERVER['QUERY_STRING'] = $tmp['query'];
		parse_str($tmp['query'], $_GET);
	} else {
		$_SERVER['QUERY_STRING'] = '';
	}

	unset($_SERVER['HTTP_X_REWRITE_URL']);
}

// расширяем корневые каталоги для подключаемых файлов - только, нахуа?
$includePath = array(
	BASEPATH.'/libpp/lib/',
	BASEPATH.'/local/lib/',
	BASEPATH,
	'.',
	BASEPATH.'/libpp/vendor',
	BASEPATH.'/libpp/vendor/PEAR',
	get_include_path(),
	'/usr/local/lib/php/'
);
set_include_path(implode(PATH_SEPARATOR, $includePath));

/*
 * Parse and load BASEPATH . /etc/php.ini
 * Some custom variables (not php ones) are:
 * 
 * profiler = true | false    # false by default
 * user = someuser            # 'nobody' by default
 * 
 */
$profiler    = false;
$system_user = 'nobody';

if (file_exists(BASEPATH.'/site/etc/php.ini')) {
	$ini_params = parse_ini_file(BASEPATH.'/site/etc/php.ini');

	foreach ($ini_params as $key=>$param) {
		ini_set($key, $param);
	}

	// Включаем профайлер
	if(isset($ini_params['profiler']) && $ini_params['profiler']) {
		$profiler = true;
	}

	if (isset($ini_params['user'])) {
		$system_user = $ini_params['user'];
	}
}

define('PROFILER', $profiler);
define('SYSTEM_USER', $system_user);

// Влючаем дебаг
error_reporting(E_ALL);
define('DEBUG', true);

// charset based defines
define('CHARSET_KOI8R',              'koi8-r');
define('CHARSET_WINDOWS',            'windows-1251');
define('CHARSET_ISO',                'iso-8859-5');
define('CHARSET_LATIN1',             'iso-8859-1');
define('CHARSET_ANSI',               'cp866');
define('CHARSET_DOS',                CHARSET_ANSI);
define('CHARSET_MAC',                'mac-cyrillic');
define('CHARSET_UTF8',               'utf-8');
define('CHARSET_UCS2BE',             'ucs-2be');
define('CHARSET_UTF16BE',            'utf-16be');

// current default proxima charset
define('DEFAULT_CHARSET',            CHARSET_KOI8R);
define('DEFAULT_LOCALE',             json_encode(array('Russian_Russia.20866', 'ru_RU.KOI8-R', 'KOI8-R')));
define('REGEX_MOD', 				 '');
//define('DEFAULT_CHARSET',            CHARSET_UTF8);
//define('DEFAULT_LOCALE',             json_encode(array('Russian_Russia.UTF-8', 'ru_RU.UTF-8', 'UTF-8')));
//define('REGEX_MOD', 				   'u'); // UTF-8 mod for regexp

if (setlocale(LC_ALL, json_decode(DEFAULT_LOCALE)) === false) {
	FatalError(sprintf('Unable to set setlocale: %s', DEFAULT_LOCALE));
}
if (function_exists('mb_internal_encoding')) {
	if (mb_internal_encoding(DEFAULT_CHARSET) === false) {
		FatalError(sprintf('Unable to internal encoding to: %s', DEFAULT_CHARSET));
	}
}

// разрешенные форматы изображений
define('ENUM_ALLOWED_IMAGETYPES', serialize(array(
	'image/gif'   => 'gif',
	'image/jpeg'  => 'jpg',
	'image/pjpeg' => 'jpg',
	'image/png'   => 'png',
	'image/x-png' => 'png'
)));


// разрешенные форматы флешей
define('ENUM_ALLOWED_FLASHTYPES', serialize(array(
	'application/x-shockwave-flash'   => 'swf'
)));


// разрешенные расширения для pathname объектов контентного уровня
define('ENUM_ALLOWED_PATHNAMES', serialize(array(
	'html',
	'htm',
	'wml',
)));

define('OBJ_FIELD_CHILDREN',         'allowed');
define('OBJ_FIELD_ORDER',            'sys_order');
define('OBJ_FIELD_PATHNAME',         'pathname');

define('PP_CHILDREN_FETCH_ALL',      1);
define('PP_CHILDREN_FETCH_SELECTED', 2);
define('PP_CHILDREN_FETCH_NONE',     3);
define('PP_CHILDREN_FETCH_PAGED',    4); 

define('DEFAULT_CHILDREN_PER_PAGE',  20);

define('P_LEFT',  1);
define('P_RIGHT', 2);


define('PX_ENGINE_USER',    1);
define('PX_ENGINE_ADMIN',   2);
define('PX_ENGINE_SBIN',    3);

define('DT_STRUCT', 'struct');

if (!IS_WIN) {
	$system_user = posix_getpwnam(SYSTEM_USER);

	switch (posix_getuid()) {

		case $system_user['uid']:
			// all ok we have target user 
			break;

		case 0:
			// root
			posix_setuid($system_user['uid']);
			posix_setgid($system_user['gid']);
			break;

		default:
			die("This script must be runned as " . SYSTEM_USER . " (uid ".$system_user['uid'].") or root. \nNow it runned as ".posix_getlogin()."(uid ".posix_getuid().")\n");
	}
}

define('HTDOCS_PATH', BASEPATH.'/site/htdocs');
define('CACHE_PATH', BASEPATH.'/tmp/cache.'.strval(PHP_VERSION_ID).'.'.strtolower(DEFAULT_CHARSET));

?>
