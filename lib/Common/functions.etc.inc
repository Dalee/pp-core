<?php
function createPathByParentId(&$tree, $id) {
	return createSomePathByParentId($tree, $id);
}

function createSomePathByParentId(&$tree, $id, $fieldname='pathname', $delim='/', $inContent=true, $withRoot=true, $in_recursion=false) {
	
	$createdPath = '';
	$path = '';
	if(isset($tree->leafs[$id])) {
		$path = $inContent ? $tree->leafs[$id]->content[$fieldname] : $tree->leafs[$id]->{$fieldname};
	}

	if(isset($tree->leafs[$id]->parent) && $tree->leafs[$id]->parent) {
		$createdPath = createSomePathByParentId($tree, $tree->leafs[$id]->parent, $fieldname, $delim, $inContent, $withRoot, true).$path.$delim;

	} elseif($withRoot) {
		$createdPath = ($path == 'default' ? '' : 'http://'.$path).$delim;

	} else {
		$createdPath = '';
	}
	
	if(!$in_recursion) {
		$app = PXRegistry::getApp();
		foreach($app->triggers->layout as $t) {
			$createdPath = $t->getTrigger()->OnAfterPathCreated($createdPath);
		}
	}
	return $createdPath;
}

function parseFloat($s) {
	if (is_numeric($s)) return floatval($s);
	$s = preg_replace('/[,.]/', '.', strval($s));
	$s = preg_replace('/[^.0-9-]/', ' ', $s);
	return floatval($s);
}

function __int32($numeric_value){
	$x = (float)$numeric_value;
	$y = pow(2,32);
	
	if ($x > $y) {
		$x = pow(2,31)-1;
	} elseif ($x >= pow(2,31)) {
		$x -= $y;
	}
	
	return (int)$x;
}

//FIXME: PXUserHTMLLayout::devBlock - it's right place for it. It is placed here because we using REALLY old version of Smarty, the valid block callback can be a global function only.
function smarty_block_dev($params, $content, &$smarty) {
	$app = & PXRegistry::getApp();
	return $app->getProperty('DEVELOPMENT_FACILITY', false) ? $content : '';
}

?>
