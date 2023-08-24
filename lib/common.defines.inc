<?php

// Default timezone settings
date_default_timezone_set('Europe/Moscow');

// Charset constants
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

// Default charset settings
define('DEFAULT_CHARSET', CHARSET_UTF8);
define('DEFAULT_LOCALE', json_encode(['Russian_Russia.UTF-8', 'ru_RU.UTF-8', 'UTF-8']));
define('REGEX_MOD', 'u'); // UTF-8 mod for regexp

if (setlocale(LC_ALL, json_decode(DEFAULT_LOCALE)) === false) {
	die(sprintf('Unable to set setlocale: %s', DEFAULT_LOCALE));
}
if (function_exists('mb_internal_encoding')) {
	if (mb_internal_encoding(DEFAULT_CHARSET) === false) {
		die(sprintf('Unable to internal encoding to: %s', DEFAULT_CHARSET));
	}
}

// CLI mode?
if (!defined('IS_CLI')) {
	define('IS_CLI', PHP_SAPI === 'cli');
}

if (IS_CLI || empty($_SERVER['DOCUMENT_ROOT'])) {
	// TODO: this will be wrong in case of vendor/pp/core
	$_SERVER['DOCUMENT_ROOT'] = realpath(__DIR__ . '/../');
}

// Определение BASEPATH - это полный путь к корню сайта
if (!defined('BASEPATH')) {
	$basePath = str_replace("/", DIRECTORY_SEPARATOR, (string) $_SERVER["DOCUMENT_ROOT"]);
	$basePath = rtrim($basePath, DIRECTORY_SEPARATOR) . DIRECTORY_SEPARATOR;
	define('BASEPATH', $basePath);
}

// Constants
define('PPCOREPATH', realpath(__DIR__ . '/../') . DIRECTORY_SEPARATOR);
define('PPLIBPATH', PPCOREPATH . 'lib' . DIRECTORY_SEPARATOR);
define('PPSERVICESPATH', PPCOREPATH . 'etc' . DIRECTORY_SEPARATOR . 'services');
define('LOCALPATH', BASEPATH . 'local' . DIRECTORY_SEPARATOR);
define('SHAREDPATH', BASEPATH . 'site' . DIRECTORY_SEPARATOR);
define('APPPATH', BASEPATH . 'app' . DIRECTORY_SEPARATOR);
define('HTDOCS_PATH', BASEPATH . '/site/htdocs' . DIRECTORY_SEPARATOR);
define('CACHE_PATH', APPPATH . 'cache/tmp.' . strval(PHP_VERSION_ID) . '.' . strtolower(DEFAULT_CHARSET));
define('RUNTIME_PATH', APPPATH . 'runtime' . DIRECTORY_SEPARATOR);

// Default include path
set_include_path(implode(PATH_SEPARATOR, [
	PPLIBPATH,
	LOCALPATH . '/lib/',
	BASEPATH,
	'.',
	PPCOREPATH . '/vendor',
	get_include_path(),
	'/usr/local/lib/php/'
]));

/*
 * Parse and load BASEPATH . /etc/php.ini
 * Some custom variables (not php ones) are:
 *
 * user = someuser            # 'nobody' by default
 *
 */
$systemUser = 'nobody';
$systemEnv = 'unknown';

if (file_exists(SHAREDPATH . '/etc/php.ini')) {
	$params = parse_ini_file(SHAREDPATH . '/etc/php.ini');
	foreach ($params as $key => $param) {
		ini_set($key, $param);
	}

	$systemUser = $params['user'] ?? $systemUser;
	$systemEnv = $params['env'] ?? $systemEnv;
}

define('SYSTEM_USER', $systemUser);
define('ENV', $systemEnv);
define('LOGGER_APP', 'application');
define('LOGGER_CRON', 'cron');

define('ENV_DEVELOPER', 'DEVELOPER');

// разрешенные форматы изображений
define('ENUM_ALLOWED_IMAGETYPES', [
	'image/gif' => 'gif',
	'image/jpeg' => 'jpg',
	'image/pjpeg' => 'jpg',
	'image/png' => 'png',
	'image/x-png' => 'png'
]);

// разрешенные расширения для pathname объектов контентного уровня
define('ENUM_ALLOWED_PATHNAMES', [
	'html',
	'htm',
	'wml',
]);

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
	$systemUser = posix_getpwnam(SYSTEM_USER);
	switch (posix_getuid()) {
		case $systemUser['uid']:
			// all ok we have target user
			break;

		case 0:
			// root
			posix_setuid($systemUser['uid']);
			posix_setgid($systemUser['gid']);
			break;

		default:
			die("This script must be run as " . SYSTEM_USER . " (uid " . $systemUser['uid'] . ") or root. \nIt's been run as " . posix_getlogin() . "(uid " . posix_getuid() . ")\n");
	}
}
