#!/usr/local/bin/php -q
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

$app     = new PXApplication(BASEPATH);
$db      = new PXDataBase($app);

foreach ($app->types as $type) {
	if (!count($type->childs)) continue;
	$tmp = $db->GetTableInfo($type->id);

	if (!isset($tmp[OBJ_FIELD_CHILDREN])) {
		echo "Altering table {$type->id}...";
		$db->ModifyingQuery("ALTER TABLE {$type->id} ADD COLUMN allowed TEXT");
		print " done\n";
	}

	$objects = $db->GetObjects($type, NULL);

	foreach ($objects as $object) {
		if (!is_array($object[OBJ_FIELD_CHILDREN]) || !count($object[OBJ_FIELD_CHILDREN])) continue;

		foreach ($object[OBJ_FIELD_CHILDREN] as $k=>$v) {
			if (!is_numeric($k)) continue(2);
		}

		print "Converting '".$object['title']."'...";
		$saved = $object[OBJ_FIELD_CHILDREN];
		$object[OBJ_FIELD_CHILDREN] = array();
		foreach ($saved as $s) {
			$object[OBJ_FIELD_CHILDREN][$s] = PP_CHILDREN_FETCH_ALL;
		}
		print " done\n";
		// print_r($object);
		$db->ModifyObjectSysVars($type, $object);
	}
}

?>
