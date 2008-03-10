#!/usr/local/bin/php -q
<?

if (count($_SERVER['argv']) < 3) {
	echo "usage: ./updTopics <pp-powered-site-dir> <forummodule>\n\n";
	exit();
}

if (!is_dir($_SERVER['argv'][1].'/libpp/')) {
	echo "Sorry, but it is not PP powered site directory\n\n";
	exit();
}

$nobody = posix_getpwnam('nobody');
if (posix_getuid() == $nobody['uid']) {
	// nobody
} else if (posix_getuid() == 0) {
	// root
	posix_setgid($nobody['gid']);
	posix_setuid($nobody['uid']);
} else {
	print 'You must be root or nobody to run this script'."\n";
	exit();
}


$_SERVER["DOCUMENT_ROOT"] = $_SERVER['argv'][1];
require_once dirname(__FILE__).'/../lib/mainadmin.inc';

$app     = new PXApplication(BASEPATH);
$db      = new PXDataBase($app);
$tree    = new NLTree($db->GetObjects($app->types['struct'], true));

$news = $app->modules[$_SERVER['argv'][2]]->settings;
$newMsg = $news['messages'];
$newTop = $news['topics'];

$topics = $db->query("SELECT * FROM {$newTop} ORDER BY id");
foreach ($topics as $t) {
	$tmp = $db->query("SELECT COUNT(*) AS k FROM {$newMsg} WHERE parent = {$t['id']}");
	$k = $tmp[0]['k'] - 1;
	$tmp = $db->query("SELECT * FROM {$newMsg} WHERE parent = {$t['id']} ORDER BY sys_created DESC LIMIT 1");
	$m = $tmp[0];
	$q = "UPDATE {$newTop} SET count = '{$k}', lastreply = '{$m['sys_created']}', lastauthor = '{$m['sys_owner']}' WHERE id = {$t['id']}";
	$db->modifyingquery($q);
}

?>
