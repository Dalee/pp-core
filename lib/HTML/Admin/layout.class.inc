<?php
class PXAdminHTMLLayout extends NLAbstractLayout {
	var $types;

	function PXAdminHTMLLayout($outerLayout, $types) {
		$this->NLAbstractLayout();
		$this->SetOuterLayout($outerLayout);
		$this->AssignTitle();
		$this->SetOuterLogo('i/admin.gif', 'Proxima CMS Портал', 250, 45);
		$this->types = $types;
	}

	function setApp() {
		// pass ;
	}

	function AssignTitle($title= NULL) {
		$title = ((is_null($title) || !strlen(trim($title))) ? '' : $title.' / ')."Proxima CMS Портал";
		$this->Assign("OUTER.TITLE", $title);
	}

	function AssignError($label, $errorText) {
		$this->Assign($label, '<p class="error">'.$errorText.'</p>');
	}

	function AssignTree($label, $objectFormat, $tree, $selectedSid, $leafsToOpen) {
		$html = $this->_MakeTree($objectFormat, $tree, $selectedSid, $leafsToOpen);
		$this->Assign($label, $html);
	}

	function AppendTree($label, $objectFormat, $tree, $selectedSid, $leafsToOpen) {
		$html = $this->_MakeTree($objectFormat, $tree, $selectedSid, $leafsToOpen);
		$this->Append($label, $html);
	}

	function _MakeTree(&$objectFormat, &$tree, $selectedSid, $leafsToOpen) {
		// Добавляем в массив "расхлопывающихся" листьев всех предков выбранного
		$tmpId = $selectedSid;
		while (isset($tree->leafs[$tmpId]->parent)) {
			$tmpId = $tree->leafs[$tmpId]->parent;
			$leafsToOpen[] = $tmpId;
		}
		// Рекурсивно генерируем дерево
		$html = '<DIV class="tree">'.$this->_MakeTreePart($tree, $objectFormat, 0, $leafsToOpen, $selectedSid).'</DIV>';
		return $html;
	}

	function _MakeTreePart(&$tree, &$objectFormat, $id, $leafsToOpen, $selectedSid, $level=0, $parent=0, $leafImgs=array(), $leafAlias=array()) {
		$html  = NULL;
		$leafs = $tree->leafs[$id]->children;
		$first = reset($leafs);
		$last  = end($leafs);

		foreach ($leafs as $leaf) {
			$haveSysOrder = strpos($objectFormat->order, 'sys_order') !== false ? true : false;
			$up   = current($leafs) == $first || !$haveSysOrder ? 0 : 1;
			$down = current($leafs) == $last  || !$haveSysOrder ? 0 : 1;

			$menuTitle = str_replace('"', '', str_replace("'", '', strip_tags($tree->leafs[$leaf]->title)));
			$context = 'Context(event, \'edit\', '.$leaf.', '.(int)($tree->leafs[$leaf]->content['status']).', \''.$objectFormat->id.'\',  \''.$menuTitle.'\', \''.implode('/', $leafAlias).'/'.'\', '.$level.', '.$up.', '.$down.'); return false;';
			$html .= '<DIV oncontextmenu="'.$context.'" id="leafId'.$leaf.'" style="';

			if ($leaf == $selectedSid) {
				$html .= 'background-color:#385A94;';
			} else {
				$html .= 'background-color:#FFFFFF;';
			}

			if (!in_array($parent, $leafsToOpen) && $level > 1) $html .= ' display: none;';

			$html .= '">';

			if ($level) {
				foreach ($leafImgs as $i) {
					$html .= '<IMG src="i/tree/'.$i.'.gif">';
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

				if (current($leafs) == $last) {
					$cLeafImg .= '-last';
				}

				if (current($leafs) == $last) {
					$leafI = 'v';
				} else {
					$leafI = 'tree';
				}

				$leafImgs[$leaf] = $leafI;
				$leafAlias[$leaf] = (isset($objectFormat->fields['pathname'])) ? $tree->leafs[$leaf]->content['pathname'] : NULL;

				if (!is_numeric(strpos($cLeafImg, 'void'))) $html .= '<A href="javascript:hideShowLeaf(\''.$leaf.'\');">';
				$html .= '<IMG src="i/tree/'.$cLeafImg.'.gif" id="leafImg'.$leaf.'">';
				if (!is_numeric(strpos($cLeafImg, 'void'))) $html .= '</A>';
			}

			$leafImg = 'leaf';
			if ($level == 0) {
				$leafImg .= (!sizeof($tree->leafs[$leaf]->children)) ? '-0' : '-1';
			} else {
				$leafImg .= (!sizeof($tree->leafs[$leaf]->children)) ? '-l' : '';
			}

			$html .= '<IMG onClick="'.$context.'" class="cursor" src="i/tree/'.$leafImg.'.gif" alt="Изменить раздел &laquo;'.$menuTitle.'&raquo;">';
			$html .= '<A href="javascript:EditContent(\''.$objectFormat->id.'\', \''.$leaf.'\')"><IMG src="i/tree/edit.gif" alt="Изменить раздел &laquo;'.$menuTitle.'&raquo;"></A>';

			if ($objectFormat->childs) {
				$varName = (isset($objectFormat->makeTreeWithSid)) ? 'sid' : 'cid'; //hack for main module
				$html .= '<A href="'.$this->_BuildHref($varName, $leaf).'">';
			}
			if ($tree->leafs[$leaf]->content['status'] == false) $html .= '<STRIKE>';
			$html .= '<SPAN onClick="ContextHide();"';
			if ($leaf == $selectedSid) $html .= ' style="color:#FFFFFF;"';
			if (isset($objectFormat->fields['pathname'])) $html .= ' title="'.$tree->leafs[$leaf]->content['pathname'].'"';
			$html .= '>';
			$html .= strip_tags($tree->leafs[$leaf]->title);
			$html .= '</SPAN>';
			if ($tree->leafs[$leaf]->content['status'] == false) $html .= '</STRIKE>';
			if ($objectFormat->childs) {
				$html .= '</A>';
			}

			next($leafs);

			$html .= $this->_MakeTreePart($tree, $objectFormat, $leaf, $leafsToOpen, $selectedSid, $level+1, $leaf, $leafImgs, $leafAlias);
			$html .= '</DIV>';
			unset($leafImgs[$leaf]);
			unset($leafAlias[$leaf]);
		}
		return $html;
	}

	function AssignContentControls($label, $selectedSid, $allowedFormats) {
		FatalError("please use PXAdminHTMLLayout::AssignControls");
	}

	function AssignControls($label, $selectedSid, $allowedFormats) {
		$html = NULL;
		foreach ($allowedFormats as $format) {
			$html .= '<A href="javascript:AddContent(\''.$format.'\', '.(int)$selectedSid.')" class="button">';
			$html .= '<IMG src="i/icon/add.gif" width="11" height="11" alt="Добавить '.$this->types[$format]->title.'" align="absmiddle" hspace="3">';
			$html .= $this->types[$format]->title;
			$html .= '</A>';
		}
		$this->Assign($label, $html);
	}

	function AssignContextMenu($label, $selectedSid, $allowedFormats) {
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
		$this->Assign($label, $html);
	}

	function _GetParentPathName($parent) {

	}

	function AppendTableAsTree($label, $objectFormat, $tree, $selected, $leafsToOpen) {
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

	function AppendTable($label, $objectFormat, $table, $selected = NULL, $varToTitle = NULL, $page=1, $objectsOnPage=0, $count=0, $withLinks=TRUE, $parentPathname=NULL) {
		$htmltable = new PXAdminTable($table, $this->types[$objectFormat], $this->getData);
		$htmltable->setPager($page, $objectsOnPage, $count);
		$this->append($label, $htmltable->getTable());
	}


	function AppendUserTable($label, $objectFormat, $title, $table, &$userClassName, $userFuncName, $page=1, $objectsOnPage=0, $count=0) {
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

	function AssignKeyValueList($label, $list, $selected, $varName = 'sid') {
		parent::AssignKeyValueList($label, $list, $varName, $selected);
	}

	function AssignIniFilesForm($label, $settings, $filename) {
		$html  = NULL;
		$html .= '<FORM method="POST" action="action.phtml">';
		$html .= '<INPUT type="hidden" name="area" value="ini">';
		$html .= '<INPUT type="hidden" name="sid" value="'.$filename.'">';

		$html .= '<TABLE width="100%" border="0" cellpadding="3" cellspacing="1">';
		$html .= '<COLGROUP bgcolor="#E7E9ED" align="center">';
		$html .= '<COL width="10%" align="center">';
		$html .= '<COL width="90%">';
		$html .= '</COLGROUP>';
		$html .= '<TR style="background-color: #C9CED6; color: #385A94;"">';
		$html .= '<TH>Значение</TH>';
		$html .= '<TH>Описание</TH>';
		$html .= '</TR>';

		$count = 0;
		foreach ($settings as $k=>$v) {
			$html .= '<TR valign="top">';
			$html .= '<TD><INPUT type="text" name="value['.$count.']" value="'.$k.'" style="width:96%;"></TD>';
			$html .= '<TD><INPUT type="text" name="descr['.$count.']" value="'.$v.'" style="width:96%;"></TD>';
			$html .= '</TR>';

			$count++;
		}

		$html .= '<TR valign="top" bgcolor="#F0F0F0">';
		$html .= '<TD><INPUT type="text" name="value['.($count+1).']" value="'.$count.'" style="width:96%;"></TD>';
		$html .= '<TD><INPUT type="text" name="descr['.($count+1).']" value="" style="width:96%;"></TD>';
		$html .= '</TR>';

		$html .= '<TR>';
		$html .= '<TD bgcolor="#C9CED6" align="right" colspan="2"><INPUT type="submit" value="Сохранить"></TD>';
		$html .= '</TR>';
		$html .= '</TABLE>';
		$html .= '</FORM>';
		$this->Assign($label, $html);
	}

}

?>
