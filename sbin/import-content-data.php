#!/usr/bin/env php5
<?php

// bootstrap
set_time_limit(0);
require_once dirname(__FILE__).'/../../libpp/lib/maincommon.inc';
if (file_exists($localcommon = BASEPATH.'local/lib/maincommon.inc')) {
	require_once ($localcommon);
}

ini_set('display_errors', '1');
$engine = new PXEngineSbin();

$app = PXRegistry::getApp();
$db = PXRegistry::getDb();

// prepare args (strip flags)
$args = $argv;
$flags = array();
foreach ($args as $k => $arg) {
	if ($arg[0] === '-') {
		empty($flags[$arg[1]]) && $flags[$arg[1]] = 0;
		$flags[$arg[1]]++;
		unset($args[$k]);
	}
}
$args = array_values(array_filter($args));


// defaults
$limit = 100;
$system_datatypes = array(
	'struct',
	'suser',
	'sgroup',
	'sys_regions',
	'adbanner',
	'adplace',
	'adcampaign'
);
$system_fields = array(
	'sys_created' => 1,
	'sys_modified' => 1,
	'sys_order' => 2,
	'sys_meta' => 3,
	'sys_owner' => 4,
);

function usage($err = null) {
	global $args;
	$err && print('Error: '.$err.PHP_EOL.PHP_EOL);
	die("Usage:\n\t${args[0]} ppdata.json ppdata.files.tar.gz".PHP_EOL.PHP_EOL);
}

isset($args[1]) || usage();
file_exists($input = $args[1]) || usage('file "'.$input.'" doesn\'t exists');

$import = json_decode(file_get_contents($input), 1);
empty($import['data']) && usage("Invalid file \"$input\"");

function wrong_schemas($err) {
	Label($err);
	Label('Check schemas please before importing data.');
	die(1);
}

// check schemas of data
$oldIdParentMap = array();
foreach ($import['data'] as $typeKey => $objects) {
	empty($app->types[$typeKey]) && wrong_schemas('Undefined datatype "'.$typeKey.'".');

	$type = $app->types[$typeKey];
	$next = 0;
	$oldIdParentMap[$typeKey] = array();
	foreach ($objects as $object) {
		$oldIdParentMap[$typeKey][$object['id']] = isset($object['parent'])? $object['parent'] : 0;
		if ($next) continue;
		foreach ($object as $k => $v) {
			// skip system fields and pass valid
			if (isset($system_fields[$k]) || isset($type->fields[$k])) {
				continue;
			}
			// throw warn if flag -i set and field of system datatype
			if (in_array($typeKey, $system_datatypes) && !empty($flags['i'])) {
				Label('Warn: Unexistent field "'.$k.'" at datatype "'.$typeKey.'".');
				$next = 1;
			// throw exception if didn't
			} else {
				wrong_schemas('Unexistent field "'.$k.'" at datatype "'.$typeKey.'".');
			}
		}
	}
}

// calculate right order
$order = array();
foreach (array_keys($import['data']) as $typeKey) {
	// d20($typeKey);
	empty($order[$typeKey]) && ($order[$typeKey] = 1);
	if ($app->types[$typeKey]->parent) {
		$pTypeKey = $app->types[$typeKey]->parent;
		empty($pTypeKey) && ($pTypeKey = 1);
		$order[$pTypeKey]++;
	}
}
uasort($order, create_function ('$a,$b', 'if ($a == $b) return 0; return $a < $b ? 1 : -1;'));

// open transaction to prevent data corruption
$db->transactionBegin();

function rollback_n_die($err = null, $object = null) {
	$err || ($err = 'Unknown error while importing data.');
	Label($err);
	$object && d2($object);

	global $db;
	$db->transactionRollback();
	die(2);
}

// try to put data
$idMap = array();
foreach ($order as $typeKey => $order) {
	empty($idMap[$typeKey]) && ($idMap[$typeKey] = array());
	$objects = $import['data'][$typeKey];
	Label(sprintf('Adding %d objects of "%s" datatype.', count($objects), $typeKey));
	$count = count($objects);
	$type = $app->types[$typeKey];
	while (!empty($objects)) {
		$object = array_shift($objects);
		$oldId = $object['id'];
		if (isset($object['parent'])) {
			if (isset($idMap[$type->parent][$object['parent']])) {
				$object['parent'] = $idMap[$type->parent][$object['parent']];
			} else if (isset($oldIdParentMap[$type->parent][$object['parent']])) {
				// $objects[$object['parent']]
				array_push($objects, $object);
				continue;
			} else {
				d20($oldIdParentMap[$type->parent]);
				rollback_n_die('Parent '.$type->parent.'#'.$object['parent'].' for object '.$typeKey.'#'.$object['id'].' not exist.', $object);
			}
		}
		unset($object['id']);
		if (isset($object['type']) && $object['type'] == 'devices' && $object['pathname'] == 'uslugi_svyazi__telefoniya_vnutri_s1') {
		//	d2($object);
		}

		$obj = array_merge($app->initContentObject($typeKey), $object);
		WorkProgress(false, $count);

		if ($type->parent && $typeKey != $type->parent) {
			$parentObj = $db->getObjectById($type->parentType(), $obj['parent']);
			if (is_array($parentObj['allowed'])) {
				$parentObj['allowed'][$typeKey] = 1;
				$db->modifyObjectSysVars($type->parentType(), $parentObj);
			}
			//die;
		}

		$idMap[$typeKey][$oldId] = $obj['id'] = @$db->addContentObject($type, $obj);
		$db->modifyObjectSysVars($type, $obj);
		if (isset($object['type']) && $object['type'] == 'devices' && $object['pathname'] == 'uslugi_svyazi__telefoniya_vnutri_s1') {
	//		d20($db->getObjectById($type->parentType(), $idMap[$typeKey][$oldId]));
	//		die;
		}
	}
	WorkProgress(true);
}

$db->transactionRollback();
die;
$db->transactionCommit();
// d20($import['data']);die;

die;

// fetch file data
$outpath = tempnam(BASEPATH.'/tmp', 'pp.export');
unlink($outpath);
MakeDirIfNotExists($outpath);
foreach ($export['data'] as $typeKey => $objects) {
	$type = $app->types[$typeKey];
	foreach ($type->fields as $fk => $field) {
		if (! $field->storageType instanceof PXStorageTypeFile) continue;
		foreach ($objects as $id => &$object) {
			if (empty($object[$fk])) continue;
			$file = HTDOCS_PATH . $object[$fk]['path'];
			if (!file_exists($file)) { // clean if no file. broken data
				$objects[$id][$fk] = null;
			} else {
				$dest = $outpath . $object[$fk]['path'];
				MakeDirIfNotExists(dirname($dest));
				copy($file, $dest);
			}
		}
	}
}
$aifilestar = $outpath . '/ppdata.files.tar.gz';

// fetch modules list

// fetch modules code - todo

// fetch modules descriptions - todo



// dump
`cd $outpath; tar -cvzf $aifilestar ./; rm -r $outpath/ai`;
echo 'files: ' . $aifilestar . PHP_EOL;

$outdata = json_encode($export);
$outfile = $outpath . '/ppdata.json';
file_put_contents($outfile, $outdata); // or dump to stdout
echo 'data json: ' . $outfile . PHP_EOL;
//echo $outdata;

