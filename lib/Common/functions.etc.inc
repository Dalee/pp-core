<?php
function createPathByParentId(&$tree, $id) {
	return createSomePathByParentId($tree, $id);
}

function createSomePathByParentId(&$tree, $id, $fieldname='pathname', $delim='/', $inContent=true, $withRoot=true) {
	if(isset($tree->leafs[$id])) {
		$path = $inContent ? $tree->leafs[$id]->content[$fieldname] : $tree->leafs[$id]->{$fieldname};
	} else {
		$path = '';
	}

	if(isset($tree->leafs[$id]->parent) && $tree->leafs[$id]->parent) {
		return createSomePathByParentId($tree, $tree->leafs[$id]->parent, $fieldname, $delim, $inContent, $withRoot).$path.$delim;

	} elseif($withRoot) {
		return ($path == 'default' ? '' : 'http://'.$path).$delim;

	} else {
		return '';
	}
}

function parseFloat($s) {
	if (is_numeric($s)) return floatval($s);
	$s = preg_replace('/[,.]/', '.', strval($s));
	$s = preg_replace('/[^.0-9-]/', ' ', $s);
	return floatval($s);
}

//FIXME: PXUserHTMLLayout::devBlock - it's right place for it. It is placed here because we using REALLY old version of Smarty, the valid block callback can be a global function only.
function smarty_block_dev($params, $content, &$smarty) {
	$app = & PXRegistry::getApp();
	return $app->getProperty('DEVELOPMENT_FACILITY', false) ? $content : '';
}

?>
