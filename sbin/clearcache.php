#!/usr/local/bin/php -q
<?
include '../lib/main.inc';

$d = new NLDir(BASEPATH.'/var/dbcache/');
$d->Emptyfy();

?>
