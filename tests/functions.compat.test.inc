<?php
/**
 * Testing lib/Common/functions.compatibility.inc
 * $ sudo -u nobody ./libpp/sbin/pp.php test:all
 *
 */
// Base string manupulation routines
t(
	'mb_ucfirst',
	function () { return mb_ucfirst('привет'); },
	'Привет'
);
t(
	'mb_ucfirst 1',
	function () { return mb_ucfirst('на Евпаторию'); },
	'На Евпаторию'
);
t(
	'mb_ucfirst 2',
	function () { return mb_ucfirst('На Евпаторию'); },
	'На Евпаторию'
);

t(
	'mb_lcfirst',
	function () { return mb_lcfirst('Привет'); },
	'привет'
);
t(
	'mb_lcfirst 1',
	function () { return mb_lcfirst('На Евпаторию'); },
	'на Евпаторию'
);
t(
	'mb_ucfirst 2',
	function () { return mb_lcfirst('на Евпаторию'); },
	'на Евпаторию'
);

