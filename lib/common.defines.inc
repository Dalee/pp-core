<?php
include_once 'Version/svn_revision.inc';

// Default timezone settings for formated date output
date_default_timezone_set('Europe/Moscow');

// include_path
set_include_path(get_include_path() . PATH_SEPARATOR . '/usr/local/lib/php/');

// Определение не на винде ли мы запущены
if(!defined('IS_WIN')) {
	define('IS_WIN', (substr(PHP_OS, 0, 3) == 'WIN'));
}

// Определение корня сайта в случае запуска скрипта из консоли (CLI)
if(!isset($_SERVER['REQUEST_METHOD'])) {
	if(IS_WIN) {
		$_SERVER['DOCUMENT_ROOT'] = realpath(dirname($_SERVER['argv'][0]).'/../../').'\\'; // ' (escaped for MC)
	} else {
		$tmp = realpath($_SERVER['SCRIPT_NAME']);

		// if ($tmp{0} !== '/') $tmp = $_SERVER['PWD'] . '/' . $tmp;


		$path = array();
		foreach (explode('/', $tmp) as $token) {
			switch ($token) {

			case '':
			case '.':
				// skip
				break;
			case '..':
				array_pop($path);
				break;
			default:
				array_push($path, $token);
			}
		}
		$path = '/' . implode('/', $path);
		$path = preg_replace('!/(?:libpp|local|site).+!', '', $path . '!!!');

		if (strpos($path, '!!!')) {
			$path = null;
			if (realpath('./libpp/') and realpath('./local')) {
				$path = realpath('.');
			}
		}

		if ($path) {
			$_SERVER['DOCUMENT_ROOT'] = $path;
		} else {
			die("Can't guess DOCUMENT_ROOT path...");
		}

		$path = realpath($path);

		$_SERVER['DOCUMENT_ROOT'] = $path;
	} //if IS_WIN
}


// Определение BASEPATH - это полный путь к корню сайта
if(!defined('BASEPATH')) {
    define('BASEPATH', $_SERVER['DOCUMENT_ROOT'].'/');
}


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

// расширяем корневые каталоги для подключаемых файлов
$includePath = array(
	BASEPATH.'/libpp/lib',
	BASEPATH.'/local/lib',
	BASEPATH,
	BASEPATH.'/libpp/vendor',
	ini_get('include_path')
);
ini_set('include_path', implode(PATH_SEPARATOR, $includePath));


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


// Выставляем правильно локаль в koi8 (в PP2 перейдем на UTF, да?)
$tmp = setlocale(LC_ALL, array('Russian_Russia.20866', 'ru_RU.KOI8-R', 'KOI8-R'));
if ($tmp === false) {
	// TODO: fire error or warning
}
unset($tmp);


// версия PP
define('PP_VERSION', '1.5');

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

define('PP_CHILDREN_FETCH_ALL',      1);
define('PP_CHILDREN_FETCH_SELECTED', 2);
define('PP_CHILDREN_FETCH_NONE',     3);
define('PP_CHILDREN_FETCH_PAGED',    4); 


define('DEFAULT_CHARSET',            'koi8-r');

define('P_LEFT',  1);
define('P_RIGHT', 2);


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
			die("\n\nThis script must be runned as " . SYSTEM_USER . " (uid ".$system_user['uid'].") or root. \nNow it runned as ".posix_getlogin()."(uid ".posix_getuid().")");
	}
}
?>
