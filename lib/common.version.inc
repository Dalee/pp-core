<?php

$defaultVersion = 'v1.5';

// версия и ревизия PP
$version  = file_exists(BASEPATH . '/libpp/VERSION')  ? file_get_contents(BASEPATH . '/libpp/VERSION')  : $defaultVersion;
$version  = trim($version);
$treeish  = preg_replace('/^.*\-(\d+\-[\d\w]+)$/', '$1', $version);
$revision = file_exists(BASEPATH . '/libpp/REVISION') ? file_get_contents(BASEPATH . '/libpp/REVISION') : '';
$revision = trim($revision);

define('PP_VERSION', $version);
define('PP_TREEISH', $treeish);
define('PP_REVISION', $revision);

