#!/usr/local/bin/php -q
<?

if (count($_SERVER['argv']) < 6) {
	echo "usage: ./migrateForum <pp-powered-site-dir> <old-forummodule> <old-id> <new-forummodule> <new-id>\n\n";
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

$olds = $app->modules[$_SERVER['argv'][2]]->settings;
$oldId = $_SERVER['argv'][3];

$news = $app->modules[$_SERVER['argv'][4]]->settings;
$newId = $_SERVER['argv'][5];

$oldMsg = $olds['messages'];
$newMsg = $news['messages'];
$newTop = $news['topics'];

// get all old data
$old = $db->query("SELECT * FROM {$oldMsg} WHERE parent = {$oldId} ORDER BY pid, id");

// rebuild new structure
$_msgidx = array();

$topics = array();
$messages = array();
foreach ($old as $num => $obj) {
	if (is_null($obj['pid'])) $obj['pid'] = 0; // workaround
	if ($obj['pid'] == 0) {
		$id = $obj['id'];
		$topics[$id] = $obj;

		$topics[$id]['parent'] = $newId;
		$topics[$id]['allowed'] = serialize(array($newMsg => 1));
		$topics[$id]['count'] = 0;
		$topics[$id]['lastauthor'] = $obj['sys_owner'];
		$topics[$id]['lastreply']  = $obj['sys_created'];

		unset($topics[$id]['id']);
		unset($topics[$id]['body']);
		unset($topics[$id]['pid']);

		$lastTid = $id;
	}

	$id = $obj['id'];
	$messages[$id] = $obj;

	if ($obj['pid'] == 0) {
		$realpid = $lastTid;
	} else {
		$realpid = $_msgidx[$messages[$id]['pid']]; // fixme
		$topics[$realpid]['count']++;
		$topics[$realpid]['lastreply']  = $obj['sys_created'];
		$topics[$realpid]['lastauthor'] = $obj['sys_owner'];
	}
	$_msgidx[$id] = $realpid;
	$messages[$id]['parent'] = $realpid;
}

$_topidx = array();
foreach ($topics as $id => $topic) {
	$q = "INSERT INTO {$newTop} (".join(', ', array_keys($topic)).") VALUES (".join(', ', array_map(array($db->db, "__mapInsertData"), array_values($topic))).")";
	$_topidx[$id] = $db->modifyingquery($q, $newTop, 'id');
}

$_msgidx = array(0 => 0);
foreach ($messages as $id => $msg) {
	$msg['parent'] = $_topidx[$msg['parent']];
	$msg['pid']    = $_msgidx[$msg['pid']];
	$q = "INSERT INTO {$newMsg} (".join(', ', array_keys($msg)).") VALUES (".join(', ', array_map(array($db->db, "__mapInsertData"), array_values($msg))).")";
	$_msgidx[$id] = $db->modifyingquery($q, $newMsg, 'id');
}

?>
