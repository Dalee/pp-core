<?php
class PXAdminAjaxTreeBranch extends PXAdminTreeBranch {

	public /* override */ function html() {
		$title = strip_tags((string) $this->leaf->title);

		$html = '';
		$html .= '<div' . $this->divAttrs() . '>';
		$html .= '<span class="tree__leaf-title">';
		$html .= $this->paddingImages();
		$html .= $this->contextMenuLink();
		$html .= $this->editLink() . $this->toChildsLink('<span' . $this->spanAttrs() . '>' . $title . '</span>');
		$html .= $this->hintHtml();
		$html .= '</span>';

		$html .= '<span class="tree__leaf-fields">' . $this->listedFields() . '</span>';

		if ($this->htmlTree->isOpenLeaf($this->leaf->id) || !$this->leaf->parent) {
			$html .= PXAdminAjaxTreeBranch::childrenHTML($this->leaf->children, $this->htmlTree, $this);
		}

		$html .= '</div>';

		return $html;
	}

	public /* override */ function divAttrs() {
		$cssClass = [
			'tree-leaf tree__leaf',
			$this->htmlTree->isSelected($this->leaf->id) ? 'selected ' : '',
			$this->htmlTree->isOpenLeaf($this->leaf->parent)  || $this->leaf->level <= 2 ? 'open' : 'close'
        ];
		$format = $this->htmlTree->datatype->id;
		return ' oncontextmenu="' . $this->contextMenu() . '" id="leafId' . $format . $this->leaf->id . '" class="' . implode(' ', $cssClass) . '" ';
	}

	public /* override */ function paddingImages() {
		if ($this->leaf->level <= 1) {
			return '';
		}
		$format = $this->htmlTree->datatype->id;
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

		$img = '<img src="i/tree/' . $type . '.gif" id="leafImg' . $format . $id . '">';

		if ($openable) {
			$img = '<a href="javascript:hideShowLeaf(\'' . $id . '\', \'' . $format . '\');">' . $img . '</a>';
		}

		return $this->getPadding() . $img;
	}

	public static function childrenHTML($children, &$ajaxHtmlTree, $parentalBranch = null) {
		if(!(is_countable($children) ? count($children) : 0)){
			return '';
		}

		if(!$parentalBranch){
			$ancestors = array_reverse($ajaxHtmlTree->getLeaf(reset($children))->getAncestors());
			$siblings = null;
			$parentalBranch = null;

			$isFirst = false;
			$isLast = false;

			foreach($ancestors as $ancestorId){
				$ancestor = $ajaxHtmlTree->getLeaf($ancestorId);

				if(is_array($siblings)){
					$isFirst = reset($siblings) == $ancestor->id;
					$isLast  = end($siblings) == $ancestor->id;
				}
				$parentalBranch = new PXAdminAjaxTreeBranch($ancestor, $ajaxHtmlTree, $parentalBranch);
				$parentalBranch->setPosition($isFirst, $isLast);

				$siblings  = $ancestor->children;
			}
		}

		$html = '';

		foreach($children as $k=>$chId) {
			$isFirst = $k == 0;
			$isLast  = $k + 1 == sizeof($children);

			$leaf = new PXAdminAjaxTreeBranch($ajaxHtmlTree->getLeaf($chId), $ajaxHtmlTree, $parentalBranch);
			$leaf->setPosition($isFirst, $isLast);
			$html .= $leaf->html();
		}
		return $html;
	}

}
