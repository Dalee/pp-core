<?php
class PXAdminHTMLLayout extends NLAbstractLayout {
	var $types;
	
	var $_scripts = array();
 
	//TODO: there must be right places in <HEAD> for this
	var $_scriptsTemplates  = array(
		':css' => array(
			':area'  => 'OUTER.FORMBEGIN',
			':proto' => '<link rel="stylesheet" href="%s" type="text/css" />'
		),
		':js'  => array(
			':area'  => 'OUTER.FORMEND',
			':proto' => '<script src="%s" type="text/javascript"></script>'
		),
		':inline_js'  => array(
			':area'  => 'OUTER.FORMEND',
			':proto' => "<script type=\"text/javascript\">\n%s\n</script>"
		),
		':inline_css'  => array(
			':area'  => 'OUTER.FORMBEGIN',
			':proto' => "<style type=\"text/css\">\n%s\n</style>"
		)
 
	);

	function PXAdminHTMLLayout($outerLayout, $types) {
		$this->NLAbstractLayout();
		$this->setOuterLayout($outerLayout);
		$this->assignTitle();
		$this->setOuterLogo('i/admin.gif', 'Proxima CMS Портал', 250, 36);
		$this->assignVersion();
		$this->types = $types;
	}

	function assignVersion() {
		$this->assign('OUTER.VERSION', sprintf('version %s.%d', PP_VERSION, PP_REVISION));
	}

	function setApp() {
		// pass ;
	}

	function assignTitle($title= NULL) {
		$title = ((is_null($title) || !strlen(trim($title))) ? '' : $title.' / ')."Proxima CMS Портал";
		$this->Assign("OUTER.TITLE", $title);
	}

	function assignError($label, $errorText) {
		$this->assign($label, '<p class="error">'.$errorText.'</p>');
	}

	function assignTree($label, $objectFormat, $tree, $selectedSid, $leafsToOpen) {
		$html = $this->_makeTree($objectFormat, $tree, $selectedSid, $leafsToOpen);
		$this->assign($label, $html);
	}

	function appendTree($label, $objectFormat, $tree, $selectedSid, $leafsToOpen) {
		$html = $this->_makeTree($objectFormat, $tree, $selectedSid, $leafsToOpen);
		$this->append($label, $html);
	}

	function _makeTree(&$objectFormat, &$tree, $selectedSid, $leafsToOpen) {
		// Добавляем в массив "расхлопывающихся" листьев всех предков выбранного
		$tmpId = $selectedSid;
		while (isset($tree->leafs[$tmpId]->parent)) {
			$tmpId = $tree->leafs[$tmpId]->parent;
			$leafsToOpen[] = $tmpId;
		}

		// Рекурсивно генерируем дерево
		return '<div class="tree">'.$this->_makeTreePart($tree, $objectFormat, 0, $leafsToOpen, $selectedSid).'</div>';
	}

	function _makeTreePart(&$tree, &$objectFormat, $id, $leafsToOpen, $selectedSid, $level=0, $parent=0, $leafImgs=array(), $leafAlias=array()) {
		$html  = '';
		$leafs = $tree->leafs[$id]->children;
		$first = reset($leafs);
		$last  = end($leafs);

		foreach ($leafs as $leaf) {
			$haveSysOrder = strpos($objectFormat->order, 'sys_order') !== false;
			$up           = (int)($haveSysOrder && $leaf != $first);
			$down         = (int)($haveSysOrder && $leaf != $last);

			if ($level) {
				$leafAlias[$leaf] = (isset($objectFormat->fields['pathname'])) ? $tree->leafs[$leaf]->content['pathname'] : NULL;
			}

			$menuTitle = str_replace('"', '', str_replace("'", '', strip_tags($tree->leafs[$leaf]->title)));
			$context = 'Context(event, \'edit\', '.$leaf.', '.(int)($tree->leafs[$leaf]->content['status']).', \''.$objectFormat->id.'\',  \''.$menuTitle.'\', \''.implode('/', $leafAlias).'/'.'\', '.$level.', '.$up.', '.$down.'); return false;';
			$html .= '<div oncontextmenu="'.$context.'" id="leafId'.$leaf.'" style="';

			if ($leaf == $selectedSid) {
				$html .= 'background-color:#385A94;';
			} else {
				$html .= 'background-color:#FFFFFF;';
			}

			if (!in_array($parent, $leafsToOpen) && $level > 1) {
				$html .= ' display: none;';
			}

			$html .= '">';

			if ($level) {
				foreach ($leafImgs as $i) {
					$html .= '<img src="i/tree/'.$i.'.gif">';
				}

				if (sizeof($tree->leafs[$leaf]->children)) {
					if (!in_array($leaf, $leafsToOpen)) {
						$cLeafImg = 'close';
					} else {
						$cLeafImg = 'open';
					}
				} else {
					$cLeafImg = 'void';
				}

				if ($leaf == $last) {
					$cLeafImg .= '-last';
				}

				if ($leaf == $last) {
					$leafI = 'v';
				} else {
					$leafI = 'tree';
				}

				$leafImgs[$leaf] = $leafI;

				if (!is_numeric(strpos($cLeafImg, 'void'))) $html .= '<a href="javascript:hideShowLeaf(\''.$leaf.'\');">';
				$html .= '<img src="i/tree/'.$cLeafImg.'.gif" id="leafImg'.$leaf.'">';
				if (!is_numeric(strpos($cLeafImg, 'void'))) $html .= '</a>';
			}

			$leafImg = 'leaf';
			if ($level == 0) {
				$leafImg .= (!sizeof($tree->leafs[$leaf]->children)) ? '-0' : '-1';
			} else {
				$leafImg .= (!sizeof($tree->leafs[$leaf]->children)) ? '-l' : '';
			}

			$html .= '<img onClick="'.$context.'" class="cursor" src="i/tree/'.$leafImg.'.gif" alt="Изменить раздел &laquo;'.$menuTitle.'&raquo;">';
			$html .= '<a href="javascript:EditContent(\''.$objectFormat->id.'\', \''.$leaf.'\')"><img src="i/tree/edit.gif" alt="Изменить раздел &laquo;'.$menuTitle.'&raquo;"></a>';

			if ($objectFormat->childs) {
				$varName = (isset($objectFormat->makeTreeWithSid)) ? 'sid' : 'cid'; //hack for main module
				$html .= '<a href="'.$this->_buildHref($varName, $leaf).'">';
			}
			if ($tree->leafs[$leaf]->content['status'] == false) $html .= '<strike>';
			$html .= '<span onClick="ContextHide();"';
			if ($leaf == $selectedSid) $html .= ' style="color:#FFFFFF;"';
			if (isset($objectFormat->fields['pathname'])) $html .= ' title="'.$tree->leafs[$leaf]->content['pathname'].'"';
			$html .= '>';
			$html .= strip_tags($tree->leafs[$leaf]->title);
			$html .= '</span>';
			if ($tree->leafs[$leaf]->content['status'] == false) $html .= '</strike>';
			if ($objectFormat->childs) {
				$html .= '</a>';
			}

			next($leafs);

			$html .= $this->_makeTreePart($tree, $objectFormat, $leaf, $leafsToOpen, $selectedSid, $level+1, $leaf, $leafImgs, $leafAlias);
			$html .= '</div>';
			unset($leafImgs[$leaf]);
			unset($leafAlias[$leaf]);
		}
		return $html;
	}

	function assignContentControls($label, $selectedSid, $allowedFormats) {
		FatalError("please use PXAdminHTMLLayout::AssignControls");
	}

	function assignControls($label, $selectedSid, $allowedFormats) {
		$this->clear($label);
		$this->appendControls($label, $selectedSid, $allowedFormats);
	}


	function appendControls($label, $selectedSid, $allowedFormats) {
		foreach ($allowedFormats as $format) {
			$button = new PXControlButton($this->types[$format]->title);
			$button->setClickCode('AddContent(\''.$format.'\', '.(int)$selectedSid.')');
			$button->setClass('add');

			$button->addToParent($label);
		}
	}

	function assignContextMenu($label, $selectedSid, $allowedFormats) {
		$html = NULL;

		if (sizeof($allowedFormats)) {
			$html .=' onContextMenu="Context(event, \'add\', '.$selectedSid;

			foreach ($this->types as $k=>$v) {
				if (in_array($k, $allowedFormats)) {
					$html .= ' , \''.$k.'\', \''.$v->title.'\'';
				}
			}

			$html .= '); return false;" ';
		}

		$this->assign($label, $html);
	}

	function appendTableAsTree($label, $objectFormat, $tree, $selected, $leafsToOpen) {
		$html = '<H2>'.$this->types[$objectFormat]->title.' ('.(count($tree->leafs)-1).') [<A href="'.$this->_buildHref($objectFormat.'_view', 'plain').'">список</A>]</H2>';
		if (count($tree->leafs) > 1) {
			$html .= '<TABLE width="100%" border="0" cellpadding="2" cellspacing="1" style="border: 1px solid '.TABLECOLOR2.'" class="objects-table">';
			$html .= '<TR><TD>';
			$html .= $this->_MakeTree($this->types[$objectFormat], $tree, $selected, $leafsToOpen);
			$html .= '</TD></TR>';
			$html .= '</TABLE>';
		} else {
			$html .= '<DIV style="padding:2px; font-weight:bold;"><IMG src="i/v.gif" border="0" width="11" height="11" alt="" align="absmiddle" hspace="3">&#8212;</DIV>';
		}
		$this->Append($label, $html);
	}

	function appendTable($label, $objectFormat, $table, $selected = NULL, $varToTitle = NULL, $page=1, $objectsOnPage=0, $count=0, $withLinks=TRUE, $parentPathname=NULL) {
		$htmltable = new PXAdminTable($table, $this->types[$objectFormat], $this->getData);
		$htmltable->setPager($page, $objectsOnPage, $count);
		$this->append($label, $htmltable->getTable());
	}


	function appendUserTable($label, $objectFormat, $title, $table, &$userClassName, $userFuncName, $page=1, $objectsOnPage=0, $count=0) {
		$this->Append($label, '<H2>'.$title.' ('.$count.')</H2>');
		$html  = NULL;
		$page  = $page ? $page : 1; // ?

		if (!count($table)) {
			$html = '';
			$this->Append($label, $html);
			return;
		}

		$html .= call_user_func(array($userClassName, $userFuncName), 'header');
		foreach ($table as $rowPos=>$row) {
			$html .= call_user_func(array($userClassName, $userFuncName), 'row', $row);
		}
		$html .= call_user_func(array($userClassName, $userFuncName), 'footer');

		if ($count > $objectsOnPage && $objectsOnPage > 0) {
			$html .= '<DIV style="padding: 2px;">';
			$html .= '<STRONG style="color: #385A94;">Страницы:</STRONG> ';

			$allPages = ceil($count/$objectsOnPage)+1;
			$start    = (ceil($page/10)-1)*10+1;
			$max      = $start + 10;

			if ($max > $allPages) {
				$max = $allPages;
			}

			if ($page > 10) {
				$prev = (ceil($start/10)-1)*10 - 9;
			}

			$last = $allPages - $start - 10;

			if ($last > 0) {
				$next = (ceil($start/10)-1)*10 + 11;
			}

			if (isset($prev)) {
				$html .= '<A href="'.$this->_BuildHref($objectFormat.'_page', $prev).'">';
				$html .= '<IMG src="i/icon/left.gif" width="4" height="7" border="0" hspace="4" alt="Страница '.$prev.'">';
				$html .= '</A>';
			}

			for ($i=$start; $i<$max; $i++) {
				$html .= '<A  style="padding: 2px 4px 2px 4px; text-decoration: none;';

				if ($i == $page) {
					$html .= 'background-color: #385A94; color: #FFFFFF; font-weight: bold;';
				}

				$html .= '" href="'.$this->_BuildHref($objectFormat.'_page', $i).'" title="Страница '.$i.'">'.$i.'</A> ';
			}

			if (isset($next) && $next > 0) {
				$html .= '<A href="'.$this->_BuildHref($objectFormat.'_page', $next).'">';
				$html .= '<IMG src="i/icon/right.gif" width="4" height="7" border="0" hspace="4" alt="Страница '.$next.'">';
				$html .= '</A>';
			}

			$html .= '</DIV>';
		}
		$this->Append($label, $html);
	}

	function assignKeyValueList($label, $list, $selected, $varName = 'sid') {
		parent::AssignKeyValueList($label, $list, $varName, $selected);
	}
	
	function assignJS($pathToScript){
		$this->_renderScript($pathToScript, ':js', true);
	}
 
	function assignInlineJS($scriptBody, $uniq = true){
		$this->_renderScript($scriptBody, ':inline_js', $uniq);
	}
 
	function assignCSS($pathToScript){
		$this->_renderScript($pathToScript, ':css', true);
	}
 
	function assignInlineCSS($scriptBody){
		$this->_renderScript($scriptBody, ':inline_css', true);
	}

	function _renderScript($body, $template, $singleton){
		if(!strlen($body) || ($singleton && isset($this->_scripts[$hash = md5($body)]))) return false;
		$this->append($this->_scriptsTemplates[$template][':area'], sprintf($this->_scriptsTemplates[$template][':proto'], $body));
		return $singleton ? ($this->_scripts[$hash] = true) : true;
	}
}
?>