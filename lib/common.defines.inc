<?php

// Default timezone settings for formated date output
date_default_timezone_set('Europe/Moscow');

// Определение не на винде ли мы запущены
if(!defined('IS_WIN')) {
	define('IS_WIN', (bool)(substr(PHP_OS, 0, 3) == 'WIN'));
}


// Определение корня сайта в случае запуска скрипта из консоли (CLI)
if(!isset($_SERVER['REQUEST_METHOD'])) {
	if(IS_WIN) {
		$_SERVER['DOCUMENT_ROOT'] = realpath(dirname($_SERVER['argv'][0]).'/../../').'\\'; // ' (escaped for MC)
	} else {
		$tmp = $_SERVER['SCRIPT_NAME'];
		if ($tmp{0} !== '/') $tmp = $_SERVER['PWD'] . '/' . $tmp;

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

		@$nobody = trim(file_get_contents($_SERVER['DOCUMENT_ROOT'].'/site/etc/boot.ini'));
		if (!$nobody) $nobody = 'nobody';
		$nobody = posix_getpwnam($nobody);
		var_dump($_SERVER['DOCUMENT_ROOT']);
		var_dump($nobody);die();

		switch (posix_getuid()) {
			case $nobody['uid']:
				// nobody
				break;
			case 0:
				// root
				posix_setgid($nobody['gid']);
				posix_setuid($nobody['uid']);
				break;
			default:
				die("You must be root or nobody to run this script\n");
		}
	}
}


// Определение BASEPATH - это полный путь к корню сайта
if(!defined('BASEPATH')) {
    define('BASEPATH', $_SERVER["DOCUMENT_ROOT"]);
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
	ini_get('include_path')
);
ini_set('include_path', implode(PATH_SEPARATOR, $includePath));

// Чтение локальных настроек php
if (file_exists(BASEPATH.'/site/etc/php.ini')) {
	$ini_params = parse_ini_file(BASEPATH.'/site/etc/php.ini');
	foreach ($ini_params as $key=>$param) {
		ini_set($key, $param);
	}
}


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
define('PP_VERSION', '1.4');
Header('X-Powered-By: Proxima Portal '.PP_VERSION);


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

define('DEFAULT_CHARSET',            'koi8-r');


?>
