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
	'mb_ucfirst',
	function () { return mb_ucfirst('на Евпаторию'); },
	'На Евпаторию'
);
t(
	'mb_ucfirst',
	function () { return mb_ucfirst('На Евпаторию'); },
	'На Евпаторию'
);
