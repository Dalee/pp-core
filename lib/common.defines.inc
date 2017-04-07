<?php

// Default timezone settings for formated date output
date_default_timezone_set('Europe/Moscow');

// Определение корня сайта в случае запуска скрипта из консоли (CLI)
if (!isset($_SERVER['REQUEST_METHOD'])) {
	$_SERVER['DOCUMENT_ROOT'] = realpath(dirname(__FILE__) . '/../../');
}

// cli
defined('IS_CLI') || define('IS_CLI', PHP_SAPI === 'cli');

// Определение BASEPATH - это полный путь к корню сайта
if (!defined('BASEPATH')) {
	$basePath = str_replace("/", DIRECTORY_SEPARATOR, $_SERVER["DOCUMENT_ROOT"]);
	$basePath = rtrim($basePath, DIRECTORY_SEPARATOR) . DIRECTORY_SEPARATOR;
	define('BASEPATH', $basePath);
}

// Объявляем PPPATH, LOCALPATH, SHAREDPATH
define('PPCOREPATH', realpath(__DIR__ . '/../') . DIRECTORY_SEPARATOR);
define('PPPATH', realpath(__DIR__ . '/../') . DIRECTORY_SEPARATOR);
define('PPLIBPATH', PPPATH . 'lib' . DIRECTORY_SEPARATOR);
define('LOCALPATH', BASEPATH . 'local' . DIRECTORY_SEPARATOR);
define('SHAREDPATH', BASEPATH . 'site' . DIRECTORY_SEPARATOR);
define('APPPATH', BASEPATH . 'app' . DIRECTORY_SEPARATOR);

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
$includePath = [
	PPLIBPATH,
	LOCALPATH . '/lib/',
	BASEPATH,
	'.',
	PPCOREPATH . '/vendor',
	PPCOREPATH . '/vendor/PEAR',
	get_include_path(),
	'/usr/local/lib/php/'
];
set_include_path(implode(PATH_SEPARATOR, $includePath));

/*
 * Parse and load BASEPATH . /etc/php.ini
 * Some custom variables (not php ones) are:
 *
 * user = someuser            # 'nobody' by default
 *
 */
$system_user = 'nobody';
$env = 'unknown';

if (file_exists(SHAREDPATH . '/etc/php.ini')) {
	$ini_params = parse_ini_file(SHAREDPATH . '/etc/php.ini');

	foreach ($ini_params as $key => $param) {
		ini_set($key, $param);
	}

	if (isset($ini_params['user'])) {
		$system_user = $ini_params['user'];
	}

	if (isset($ini_params['env'])) {
		$env = $ini_params['env'];
	}
}

define('SYSTEM_USER', $system_user);
define('ENV', $env);
define('LOGGER_APP', 'application');
define('LOGGER_CRON', 'cron');

// Влючаем дебаг
error_reporting(E_ALL);
define('DEBUG', true);
define('ENV_DEVELOPER', 'DEVELOPER');

// charset based defines
define('CHARSET_KOI8R', 'koi8-r');
define('CHARSET_WINDOWS', 'windows-1251');
define('CHARSET_ISO', 'iso-8859-5');
define('CHARSET_LATIN1', 'iso-8859-1');
define('CHARSET_ANSI', 'cp866');
define('CHARSET_DOS', CHARSET_ANSI);
define('CHARSET_MAC', 'mac-cyrillic');
define('CHARSET_UTF8', 'utf-8');
define('CHARSET_UCS2BE', 'ucs-2be');
define('CHARSET_UTF16BE', 'utf-16be');

define('DEFAULT_CHARSET', CHARSET_UTF8);
define('DEFAULT_LOCALE', json_encode(array('Russian_Russia.UTF-8', 'ru_RU.UTF-8', 'UTF-8')));
define('REGEX_MOD', 'u'); // UTF-8 mod for regexp

if (setlocale(LC_ALL, json_decode(DEFAULT_LOCALE)) === false) {
	die(sprintf('Unable to set setlocale: %s', DEFAULT_LOCALE));
}

if (function_exists('mb_internal_encoding')) {
	if (mb_internal_encoding(DEFAULT_CHARSET) === false) {
		die(sprintf('Unable to internal encoding to: %s', DEFAULT_CHARSET));
	}
}

// разрешенные форматы изображений
define('ENUM_ALLOWED_IMAGETYPES', serialize(array(
	'image/gif' => 'gif',
	'image/jpeg' => 'jpg',
	'image/pjpeg' => 'jpg',
	'image/png' => 'png',
	'image/x-png' => 'png'
)));


// разрешенные форматы флешей
define('ENUM_ALLOWED_FLASHTYPES', serialize(array(
	'application/x-shockwave-flash' => 'swf'
)));


// разрешенные расширения для pathname объектов контентного уровня
define('ENUM_ALLOWED_PATHNAMES', serialize(array(
	'html',
	'htm',
	'wml',
)));

/**Вернуть данные и таблицы в виде массива*/
define('DB_SELECT_TABLE', 1);

/**Вернуть количество строк COUNT()*/
define('DB_SELECT_COUNT', 2);

/**Вернуть данные и таблицы в виде дерева, исключая orphaned ветки */
define('DB_SELECT_TREE', 4);

/**Вернуть данные и таблицы в виде LIST*/
define('DB_SELECT_LIST', 8);

/**Вернуть данные и таблицы в виде дерева, включая orphaned ветки в корневой раздел*/
define('DB_SELECT_FULL_TREE', 16);

define('DB_SELECT_WITHOUT_EXTERNALS', 32);

/**Значение не NULL*/
define('NOT_NULL', '~~___IS_NOT_NULL___~~');

define('OBJ_FIELD_ID', 'id');
define('OBJ_FIELD_CHILDREN', 'allowed');
define('OBJ_FIELD_ORDER', 'sys_order');
define('OBJ_FIELD_PATHNAME', 'pathname');
define('OBJ_FIELD_UUID', 'sys_uuid');
define('OBJ_FIELD_CREATED', 'sys_created');
define('OBJ_FIELD_MODIFIED', 'sys_modified');
define('OBJ_FIELD_META', 'sys_meta');
define('OBJ_FIELD_OWNER', 'sys_owner');
define('OBJ_FIELD_TITLE', 'title');

define('PP_CHILDREN_FETCH_ALL', 1);
define('PP_CHILDREN_FETCH_SELECTED', 2);
define('PP_CHILDREN_FETCH_NONE', 3);
define('PP_CHILDREN_FETCH_PAGED', 4);

define('DEFAULT_CHILDREN_PER_PAGE', 20);

define('P_LEFT', 1);
define('P_RIGHT', 2);


define('PX_ENGINE_USER', 1);
define('PX_ENGINE_ADMIN', 2);
define('PX_ENGINE_SBIN', 3);

define('DT_STRUCT', 'struct');
define('DT_PROPERTIES', 'sys_properties');

if (getenv('PP_DONT_FORCE_SUDO')) {
	define('PP_DONT_FORCE_SUDO', 1);
}

if (!defined('PP_DONT_FORCE_SUDO')) {
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
			die("This script must be run as " . SYSTEM_USER . " (uid " . $system_user['uid'] . ") or root. \nIt's been run as " . posix_getlogin() . "(uid " . posix_getuid() . ")\n");
	}
}

define('HTDOCS_PATH', BASEPATH . '/site/htdocs');
define('CACHE_PATH', BASEPATH . '/tmp/cache.' . strval(PHP_VERSION_ID) . '.' . strtolower(DEFAULT_CHARSET));
