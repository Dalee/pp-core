#!/usr/bin/env php54
<?php

// bootstrap
set_time_limit(0);
require_once dirname(__FILE__).'/../../libpp/lib/maincommon.inc';
if (file_exists($localcommon = BASEPATH.'local/lib/maincommon.inc')) {
	require_once ($localcommon);
}

function mime_type($filename) {
	if (is_callable('mime_content_type')) {
		list($mime) = explode(";", mime_content_type($filename));
	} elseif (is_callable('finfo_file')) {
		$finfo = finfo_open(FILEINFO_MIME_TYPE);
		$mime = finfo_file($finfo, $filename);
		finfo_close($finfo);
	} else {
		die ('Can\'t determine mime type at '.__FILE__.':'.__LINE__);
	}
	return $mime;
}

if (!is_callable('mb_strtolower')) {
	function mb_strtolower($str) {
		return strtolower($str);
	}
}

ini_set('display_errors', '1');
$engine = new PXEngineSbin();

$app = PXRegistry::getApp();
$db = PXRegistry::getDb();

// prepare args (strip flags)
$_param_names = array('p' => 'prefix',);
$_flag_names = array('i' => 'ignore-missed-fields',);
$args = array();
$flags = array();
$params = array();
reset($args);
$i = 0;
while (($arg = next($argv)) !== false) {
	$i ++;
	$k = key($args);
	if ($arg[0] === '-') {
		if (isset($_param_names[$arg[1]])) {
			empty($argv[$k+1]) && usage('Missed value for parameter '.$arg[0].$arg[1]);
			$params[$_param_names[$arg[1]]] = next($argv);
		} elseif (isset($_flag_names[$arg[1]])) {
			empty($flags[$_flag_names[$arg[1]]]) && $flags[$_flag_names[$arg[1]]] = 0;
			$flags[$_flag_names[$arg[1]]]++;
		}
	} else {
		$args[] = $arg;
	}
	($i < 15) || die('too many params');
}
array_unshift($args, $argv[0]);

// defaults
$limit = 100;
$system_datatypes = array(
	'struct',
	'suser',
	'sgroup',
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
	'allowed' => 5
);
$system_struct_types = array(
	'redirect' => 1
);

function usage($err = null) {
	global $args;
	$err && print('Error: '.$err.PHP_EOL.PHP_EOL);
	die("Usage:\n\t${args[0]} ppdata.json ppdata.files.tar.gz".PHP_EOL.PHP_EOL);
}

isset($args[1]) || usage();
file_exists($input = $args[1]) || usage('file "'.$input.'" doesn\'t exists');

$input_files = null;
isset($args[2]) && file_exists($input_files = $args[2]) && ($input_files = realpath($input_files));

$import = json_decode_koi(file_get_contents($input), 1);
empty($import['data']) && usage("Invalid file \"$input\"");

function wrong_schemas($err) {
	Label($err);
	Label('Check schemas please before importing data.');
	die(1);
}

$additionalFieldsMap = array(
	'sys_regions' => array(
		array('name' => 'sys_reflex_id', 'storagetype' => 'integer')
	)
);

function addPseudoField($app, $type, $data) {
	$attr = new SimpleXMLElement("<attribute/>");
	foreach($data as $k => $v) {
		$attr->addAttribute($k, $v);
	}
	$field = new PXFieldDescription(new PXmlSimplexmlNode($attr), $app, $type);
	$type->addField($field);
}


// sync sys_regions
$sys_regions_map = false;
if (isset($import['sys_data']['sys_regions'])) {
	if (empty($app->types['sys_regions'])) {
		wrong_schemas('System regions relation doesn\'t exist in the local database. You should fix it manually.');
	}
	Label ('Syncing system regions data');
	$outer_sys_regions = $import['sys_data']['sys_regions'];
	$inner_sys_regions = $db->getObjects($app->types['sys_regions'], null);
	$sys_regions_map = array();

	$hardcoded_alias_sets = array(
		array('МегаФон в России', 'МегаФон Головной офис'),
		array('Республика Карачаево-Черкесия', 'Республика Карачаево-Черкессия'),
		array('Норильск и Таймырский МР', 'Таймырский МР'),
	);
	foreach ($outer_sys_regions as $outer_region) {
		$found_alias_set = false;
		foreach ($hardcoded_alias_sets as $alias_set) {
			if (in_array($outer_region['title'], $alias_set)) {
				$found_alias_set = $alias_set;
				break;
			}
		}
		foreach ($inner_sys_regions as $inner_region) {
			// if titles equal or title alias equal to inner region title
			if (mb_strtolower($inner_region['title']) != mb_strtolower($outer_region['title']) && !($found_alias_set && in_array($inner_region['title'], $found_alias_set))) {
				continue;
			}
			$sys_regions_map[$outer_region['id']] = $inner_region['id'];
			break;
		}
		// fail back
		if (empty($sys_regions_map[$outer_region['id']])) {
			Label('Region was not found in the local database');
			d20($outer_region, $found_alias_set);die;
		}
	}
}

/*Label('Synchronization results:');
foreach ($sys_regions_map as $oldreg => $newreg) {
	Label(sprintf('%2.d.%-30.s - %2.d.%-30.s', $oldreg, $outer_sys_regions[$oldreg]['title'], $newreg, $inner_sys_regions[$newreg]['title']));
}
//*/

// check schemas of data
$oldIdParentMap = array();
$oldIdSysReflexIdMap = array();
Label('Checking for scheme compatibility');
foreach ($import['data'] as $typeKey => $objects) {
	empty($app->types[$typeKey]) && wrong_schemas('Undefined datatype "'.$typeKey.'".');

	$type = $app->types[$typeKey];

	//append extra fields like sys_reflex_id to selection
	foreach($additionalFieldsMap as $k => $v) {
		if (isset($type->fields[$k])) {
			foreach($v as $pseudoField) {
				addPseudoField($app, $type, $pseudoField);
			}
		}
	}
	
	$next = 0;
	$oldIdParentMap[$typeKey] = array();
	foreach ($objects as $object) {
		$oldIdParentMap[$typeKey][$object['id']] = isset($object['parent'])? $object['parent'] : 0;
		$oldIdSysReflexIdMap[$typeKey][$object['id']] = isset($object['sys_reflex_id'])? $object['sys_reflex_id'] : 0;
		if ($next) continue;
		foreach ($object as $k => $v) {
			// skip system fields and pass valid
			if (isset($system_fields[$k]) || isset($type->fields[$k])) {
				continue;
			}
			// throw warn if flag -i set and field of system datatype
			if (in_array($typeKey, $system_datatypes) && !empty($flags['ignore-missed-fields'])) {
				Label('Warn: Unexistent field "'.$k.'" at datatype "'.$typeKey.'".');
				$next = 1;
			// throw exception if didn't
			} else {
				wrong_schemas('Unexistent field "'.$k.'" at datatype "'.$typeKey.'".');
			}
		}
	}
}
if (isset($import['data']['struct'])) {
	$structTypes = $app->directory['struct-type']->values;
	$templatepath = LOCALPATH.'templates/lt/';
	$objects = & $import['data']['struct'];
	foreach ($objects as $id => $object) {
		$stype = $object['type'];
		if (isset($system_struct_types[$stype])) {
			continue;
		}
		if (!empty($params['prefix']) && isset($structTypes[$params['prefix'].'/'.$stype])) {
			$stype = $params['prefix'].'/'.$stype;
		}
		if (!isset($structTypes[$stype])) {
			$err = 'Missed struct type "'.$object['type'].'" at struct#'.$object['id'];
			empty($flags['ignore-missed-fields'])? wrong_schemas($err) : Label('Warn: '.$err);
		}
		if (!file_exists($templatepath.$stype.'.tmpl')) {
			$err = 'Missed template for struct type "'.$object['type'].'" at struct#'.$object['id'];
			empty($flags['ignore-missed-fields'])? wrong_schemas($err) : Label('Warn: '.$err);
		}
		// MAGIC BUG in php appears ahead in 'while (!empty($objects)) {$object = array_shift($objects); ...', 
		// if using 'foreach ($objects as $id => &$object) { ... }' statement, strange simultaneous array corruption
		// FUCK php! C# forever ;)
		$objects[$id]['type'] = $stype;
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

// fetch file data
if ($input_files) {
	$outpath = tempnam(BASEPATH.'/tmp', 'pp.import');
	unlink($outpath);
	MakeDirIfNotExists($outpath);
	Label('Extracting files archive to '.$outpath);

	$e_outpath = escapeshellarg($outpath);
	$e_input_files = escapeshellarg($input_files);
	`cd $e_outpath && tar -xf $e_input_files`;
	Label('Done');
}

// try to put data
Label('Pushing data to the database');
$idMap = array();
foreach ($order as $typeKey => $order) {
	empty($idMap[$typeKey]) && ($idMap[$typeKey] = array());
	$objects = array_values($import['data'][$typeKey]);
	$count = count($objects);
	Label(sprintf('Adding %d objects of "%s" datatype.', $count, $typeKey));
	
	$type = $app->types[$typeKey];

	// calc file fields
	$file_fields = array();
	foreach ($type->fields as $fk => $field) {
		if (! ($field->storageType instanceof PXStorageTypeFile) ) continue;
		$file_fields[] = $fk;
	}

	while (!empty($objects)) {
		$object = array_shift($objects);
		$oldId = $object['id'];

		// update parent field if exist
		if (isset($object['parent']) && empty($object['__parent__fixed'])) {
			if (isset($idMap[$type->parent][$object['parent']])) {
				$object['parent'] = $idMap[$type->parent][$object['parent']];
				$object['__parent__fixed'] = true;
			} else if (isset($oldIdParentMap[$type->parent][$object['parent']])) {
				$objects[] = $object;
				continue;
			} else {
				d20($oldIdParentMap[$type->parent]);
				rollback_n_die('Parent '.$type->parent.'#'.$object['parent'].' for object '.$typeKey.'#'.$object['id'].' not exist.', $object);
			}
		}

		// update sys_reflex_id field
		if (!empty($object['sys_reflex_id']) && empty($object['__sys_reflex_id__fixed'])) {
			if (isset($idMap[$typeKey][$object['sys_reflex_id']])) {
				$object['sys_reflex_id'] = $idMap[$typeKey][$object['sys_reflex_id']];
				$object['__sys_reflex_id__fixed'] = true;
			} else if (isset($oldIdSysReflexIdMap[$typeKey][$object['sys_reflex_id']])) {
				$objects[] = $object;
				continue;
			} else {
				d20($oldIdSysReflexIdMap[$type->id]);
				rollback_n_die('SysReflexId '.$typeKey.'#'.$object['sys_reflex_id'].' for object '.$typeKey.'#'.$object['id'].' not exist.', $object);
			}
		}

		// update sys_regions field
		if (isset($object['sys_regions'])) {
			foreach ($object['sys_regions'] as $k => $oldRegionId) {
				$object['sys_regions'][$k] = isset($sys_regions_map[$oldRegionId])? $sys_regions_map[$oldRegionId] : null;
			}
			$object['sys_regions'] = array_values(array_filter($object['sys_regions']));
		}
		
		// and drop id before creating new object
		unset($object['id']);

		$obj = array_merge($app->initContentObject($typeKey), $object);
		WorkProgress(false, $count);

		// tryharded fixup allowed field
		if ($type->parent && $typeKey != $type->parent) {
			$parentObj = $db->getObjectById($type->parentType(), $obj['parent']);
			if (is_array($parentObj['allowed'])) {
				$parentObj['allowed'][$typeKey] = 1;
				$db->modifyObjectSysVars($type->parentType(), $parentObj);
			}
		}

		/* array (
			[name] => 'cat_grass_and_green_background-1024x768.jpg'
			[type] => 'image/jpeg'
			[tmp_name] => '/var/tmp/phpVXi96P' ) */
		// prepare files
		unset ($obj['sys_meta']);
		foreach ($file_fields as $fk) {
			if (empty($object[$fk]) || empty($object[$fk]['path'])) continue;

			$input_file = $outpath . $object[$fk]['path'];
			if (!file_exists($input_file) || !is_readable($input_file)) continue;

			$obj[$fk] = array( // emulate php file array
				'name' => pathinfo($input_file, PATHINFO_FILENAME),
				'type' => mime_type($input_file),
				'tmp_name' => $input_file,
				'size' => filesize($input_file),
				'error' => 0
			);
		}

		// add content object
		$idMap[$typeKey][$oldId] = $obj['id'] = @$db->addContentObject($type, $obj);
		$db->modifyObjectSysVars($type, $obj);
		/*FUCK THIS SHIT! Save the fuckin sys_reflex_id anyway!!!
		if (!empty($object['sys_reflex_id'])) { // untested behaviour! checkit up asap
			$objx = $db->getObjectById($type, $obj['id']);
			d20($objx);
			rollback_n_die();
		}*/

	}
	WorkProgress(true);
}
Label('Done');


//$db->transactionRollback();
//die;
$db->transactionCommit();
// d20($import['data']);die;

die; //DIE MOTHERFUCKER DIE!!!


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

$outdata = json_encode_koi($export);
$outfile = $outpath . '/ppdata.json';
file_put_contents($outfile, $outdata); // or dump to stdout
echo 'data json: ' . $outfile . PHP_EOL;
//echo $outdata;

