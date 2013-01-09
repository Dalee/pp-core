<?php
class PXAdminAjaxTreeBranch extends PXAdminWidget {
	function PXAdminTreeBranch(&$leaf, &$htmlTree) {
		$this->leaf     =& $leaf;
		$this->htmlTree =& $htmlTree;
		$this->parent   =& $parentLeaf;
	}

	function html() {
		$title = strip_tags($this->leaf->title);
				
		$html = '';
		$html .= '<div'.$this->divAttrs().'>';

		$html .= $this->paddingImages();

		$html[] = $this->contextMenuLink();

		$html[] = $this->editLink().$this->toChildsLink('<span'.$this->spanAttrs().'>'.$title.'</span>');

		($this->htmlTree->isOpenLeaf($this->leaf->id) || !$this->leaf->parent) && $html[] = PXAdminTreeBranch::childrenHTML($this->leaf->children, $this->htmlTree, $this);

		$html[] = '</div>';


		return join($html);
	}

	function divAttrs() {
		$cssClass = array(
			'tree-leaf',
			$this->htmlTree->isSelected($this->leaf->id) ? 'selected ' : '',
			$this->htmlTree->isOpenLeaf($this->leaf->parent)  || $this->leaf->level <= 2 ? 'open' : 'close'
		);

		return ' oncontextmenu="'.$this->contextMenu().'" id="leafId'.$this->leaf->id.'" class="'.implode(' ', $cssClass).'" ';
	}

	function listedFields() {
		$fields = array();

		foreach($this->htmlTree->datatype->fields as $name=>$descr) {
			if($name !== 'title' && $descr->listed) {
				$param = array();
				$fields[] = '<dt>'.$descr->description.'</dt><dd>'.$descr->displayType->buildTreeHover($descr, $this->leaf->content, $param).'</dd>';
			}
		}

		if(sizeof($fields)) {
			return sprintf("<dl class='tree-fullBrief'>%s</dl>", implode('', $fields));

		} else {
			return '';
		}
	}

	function spanAttrs() {
		$_  = ' onClick="ContextHide();"';
		$_ .= $this->leaf->content['status'] ? '' : ' class="hide"';

		if (isset($this->htmlTree->datatype->fields['pathname'])) {
			$_ .= ' title="'.$this->leaf->content['pathname'].'"';
		}

		return $_;
	}

	function contextMenuLink() {
		$openable = sizeof($this->leaf->children);

		if ($this->leaf->level == 1) {
			$type = !$openable ? '-0' : '-1';

		} else {
			$type = !$openable ? '-l' : '';
		}

		$alt = 'Изменить раздел &laquo;'.$this->escapeTitle().'&raquo;';

		return '<img onClick="'.$this->contextMenu().'" onContextMenu="'.$this->contextMenu().'" class ="cursor" src="i/tree/leaf'.$type.'.gif" alt="'.$alt.'">';
	}

	function getPadding() {
		$imgs = array();

		$parent = $this->parent;

		while(!is_null($parent) && is_a($parent, 'PXAdminTreeBranch')) {
			$type = $parent->isLast ? 'v' : 'tree';

			if($parent->leaf->level > 1) {
				array_unshift($imgs, '<img src="i/tree/'.$type.'.gif">');
			}

			$parent = $parent->parent;
		}

		return implode('', $imgs);
	}

	function paddingImages() {
		if ($this->leaf->level <= 1) {
			return '';
		}

		$id = $this->leaf->id;

		$openable = sizeof($this->leaf->children);

		if ($openable) {
			$type = $this->htmlTree->isOpenLeaf($id) ? 'open' : 'close';

		} else {
			$type = 'void';
		}

		if ($this->isLast) {
			$type .= '-last';
		}

		$img = '<img src="i/tree/'.$type.'.gif" id="leafImg'.$id.'">';

		if ($openable) {
			$img = '<a href="javascript:hideShowLeaf(\''.$id.'\');">'.$img.'</a>';
		}

		return $this->getPadding().$img;
	}

	public static function childrenHTML($children, $ajaxHtmlTree) {
		$html = '';

		foreach($children as $k=>$chId) {
			$isFirst = $k == 0;
			$isLast  = $k + 1 == sizeof($children);

			$leaf = new PXAdminAjaxTreeBranch($htmlTree->getLeaf($chId), $ajaxHtmlTree);
			$leaf->setPosition($isFirst, $isLast);
			$html .= $leaf->html();
		}

		return $html;
	}

	function toChildsLink($html) {
		$href = $this->htmlTree->childsLink($this->leaf->id);

		if(strlen($href)) {
			$_ = $this->leaf->content['status'] ? '' : ' class="hide"';
			$html = '<a href="'.$href.'"'.$_.'>'.$html.'</a>';
		}

		return $html;
	}

	function escapeTitle() {
		return str_replace(array('"', "'"), '', strip_tags($this->leaf->title));
	}

	function editLink() {
		$alt    = 'Изменить раздел &laquo;'.$this->escapeTitle().'&raquo;';
		$format = $this->htmlTree->datatype->id;
		$id     = $this->leaf->id;

		$html = <<<HTML
			<a href="popup.phtml?area=objects&format={$format}&id={$id}&action=main" onclick="EditContent('{$format}', '{$id}'); return false;"><img src="i/tree/edit.gif" alt="{$alt}"></a>
HTML;

		return trim($html);
	}
}
?>