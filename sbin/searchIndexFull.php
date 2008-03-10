#!/usr/local/bin/php
<?

if (count($_SERVER['argv']) < 2) {
	echo "Please, give me directory name as argument\n\n";
	exit();
}

if (!is_dir($_SERVER['argv'][1].'/libpp/')) {
	echo "Sorry, but it is not PP powered site directory\n\n";
	exit();
}

$_SERVER["DOCUMENT_ROOT"] = $_SERVER['argv'][1];
require_once dirname(__FILE__).'/../lib/mainadmin.inc';
require_once dirname(__FILE__).'/../lib/cronruns/searchindex.cronrun.inc';

$app     = new PXApplication(BASEPATH);
$db      = new PXDataBase($app);
$tree    = new NLTree($db->GetObjects($app->types['struct'], true));

$indexer = new PXCronRunSearchIndex();
$indexer->_reindex($app, $db, true, false);

?>
