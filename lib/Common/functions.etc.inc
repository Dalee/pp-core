<?php
function createPathByParentId($tree, $id) {
	return createSomePathByParentId($tree, $id);
}

function createSomePathByParentId($tree, $id, $fieldname='pathname', $delim='/', $inContent=true, $withRoot=true, $in_recursion=false) {
	if (!$in_recursion) {
		$app = PXRegistry::getApp();

		foreach ($app->triggers->layout as $t) {
			$t->getTrigger()->OnBeforePathCreated($tree, $id);
		}
	}

	if (isset($tree->leafs[$id])) {
		if ($inContent) {
			$path = $tree->leafs[$id]->content[$fieldname] ?? '';
		} else {
			$path = $tree->leafs[$id]->{$fieldname} ?? '';
		}
	} else {
		$path = '';
	}

	if (isset($tree->leafs[$id]->parent) && $tree->leafs[$id]->parent) {
		$createdPath = createSomePathByParentId($tree, $tree->leafs[$id]->parent, $fieldname, $delim, $inContent, $withRoot, true).$path.$delim;

	} elseif ($withRoot && !empty($path)) {
		$createdPath = ($path == 'default' ? '' : (PXRequest::GetHttpProto().'://'.$path)).$delim;

	} else {
		$createdPath = '';
	}

	if (!$in_recursion) {
		$app = PXRegistry::getApp();
		foreach($app->triggers->layout as $t) {
			$createdPath = $t->getTrigger()->OnAfterPathCreated($createdPath);
		}
	}

	return $createdPath;
}

function parseFloat ($s) {
	if (is_numeric($s)) {
		return floatval($s);
	}

	$s = preg_replace('/[,.]/'.REGEX_MOD, '.', strval($s));
	$s = preg_replace('/[^.0-9-]/'.REGEX_MOD, ' ', $s);

	return floatval($s);
}

/**
 * parseBool
 * parses string and returns true for one of 1, "on", "yes", "true" values or false for 0, "off", "no", "false"
 * @param string $s string to parse
 * @return bool|null null if can't parse correctly which can be easily typecasted to false
 */
function parseBool ($s) {
	if (!is_scalar($s)) {
		return null;
	}

	$s = strtolower($s);
	if (in_array($s, ['1', 'on', 'y', 'yes', 'true'])) {
		return true;
	} elseif (in_array($s, ['', '0', 'off', 'n', 'no', 'false'])) {
		return false;
	}

	return null;
}

function __int32($numeric_value){
	$x = (float)$numeric_value;
	$y = 2 ** 32;

	if ($x > $y) {
		$x = 2 ** 31-1;
	} elseif ($x >= 2 ** 31) {
		$x -= $y;
	}

	return (int)$x;
}
