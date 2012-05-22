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

	$createdPath = '';
	$path = '';
	if(isset($tree->leafs[$id])) {
		$path = $inContent ? $tree->leafs[$id]->content[$fieldname] : $tree->leafs[$id]->{$fieldname};
	}

	if(isset($tree->leafs[$id]->parent) && $tree->leafs[$id]->parent) {
		$createdPath = createSomePathByParentId($tree, $tree->leafs[$id]->parent, $fieldname, $delim, $inContent, $withRoot, true).$path.$delim;

	} elseif($withRoot && !empty($path)) {
		$createdPath = ($path == 'default' ? '' : (PXRequest::GetHttpProto().'://'.$path)).$delim;

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


// temporary struct root fixer
function fillRootPathnames(&$tree, $host = null, $aliases = null) {
	// set current host
	is_null($host) && $host = PXRegistry::getRequest()->getHTTPHost();
	$app = PXRegistry::getApp();
	if (is_null($aliases) && isset($app->modules['main']->settings)) {
		$settings = $app->modules['main']->settings;
		if (isset($settings['domainAlias']) && isset($settings['rootFormat']) && 'struct' /* $format->id */ == $settings['rootFormat']) {
			$aliases = mainParseAliasesConfig($settings['domainAlias']);
		}
	}
	fillRootPathnamesWithDomainAliases(&$tree, $host, $aliases);
}

function fillRootPathnamesWithDomainAliases(&$tree, $host = null, $aliases = null) {
	// "пустой" проект
	if(!isset($tree->levels[1])) {
		return;
	}

	// находим соответствие между хостом, на который мы зашли - use case?
	// и корнем в дереве, который может быть нам нужен
	$hostAlias = 'default';
	if (isset($aliases[$host])) {
		$hostAlias = $aliases[$host];
	}

	// если алиас "не нашли", может быть нужный хост есть среди корней? - use case?
	if ($hostAlias == 'default') {
		foreach ($tree->levels[1] as $_rootId) {
			if ($tree->leafs[$_rootId]->content['pathname'] === $host) {
			$hostAlias = $host;
				break;
			}
		}
	}

	// находим нужный корень и правим его pathname
	foreach ($tree->levels[1] as $_rootId) {
		if ($tree->leafs[$_rootId]->content['pathname'] === $hostAlias) {
			$tree->leafs[$_rootId]->content['__pathname'] = $tree->leafs[$_rootId]->content['pathname'];
			$tree->leafs[$_rootId]->content['pathname'] = $host;
			break;
		}
	}
}

function mainParseAliasesConfig($aliasList) {
	if (empty($aliasList)) {
		return array();
	}

	if (is_string($aliasList)) {
		$aliasList = explode("\n", $aliasList);
	}

	$currentHost = PXRegistry::getRequest()->getHttpHost();

	$result = array();
	foreach ($aliasList as $row) {
		list ($host, $alias) = preg_split('/\s*=\s*/', trim($row));

		if (strpos($host, '*') === false) {
			$result[$host] = $alias;
			continue;
		}

		$pattern = preg_quote(trim($host));
		$pattern = str_replace('\*', '([a-z0-9\-]+)', $pattern);
		$pattern = '/^'.$pattern.'$/i';

		if (preg_match($pattern, $currentHost, $matches) && count($matches)) {
			$result[$currentHost] = $alias;
		}
	}

	return $result;
}

?>
