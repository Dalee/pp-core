<?
function createPathByParentId(&$tree, $id) {
	return createSomePathByParentId($tree, $id);
}

function createSomePathByParentId(&$tree, $id, $fieldname='pathname', $delim='/', $inContent=true, $withRoot=true) {
	if ($id == -1) { // special case. Section not found. Return default value
		if (!$withRoot) return '';
		$tmp = $tree->leafs[$tree->leafs[0]->children[0]];
		$tmp = $inContent ? $tmp->content[$fieldname] : $tmp->{$fieldname};
		return ($tmp == 'default' ? '' : 'http://' . $tmp) . $delim;
	}

	$path = $inContent ? $tree->leafs[$id]->content[$fieldname] : $tree->leafs[$id]->{$fieldname};

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

?>
