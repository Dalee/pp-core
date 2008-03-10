#!/usr/local/bin/php -q
<?
include '../lib/mainadmin.inc';

$app     = new PXApplication(BASEPATH);
$db      = new PXDataBase($app);
$request = new PXRequest();
// $user    = new PXUser($request->GetAuthData());
// $db->CheckAndFillUser($user);

$app->modules['advert']->load();
$tmp = $app->modules['advert']->class;
$ad = new $tmp('advert', $app->modules['advert']->settings);
$ad->adInterchange($app, $db);

$d = new NLDir(BASEPATH.'/var/ad/in/');

while($file = $d->readFull()) {
	chown($file, 99);
	chgrp($file, 99);
}


?>
