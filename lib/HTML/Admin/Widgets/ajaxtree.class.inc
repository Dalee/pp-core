<?php

require_once PPLIBPATH . 'HTML/Admin/Widgets/ajaxTreeBranch.class.inc';

class PXAdminAjaxTree extends PXAdminTree {
	protected $currentLeaf;
	/**
	 * Конструктор класса PXAdminAjaxTree.
	 * Инициализирует поля класса. необходимые для построения дерева.
	 */
	public function __construct(&$datatype, &$tree, $currentLeafId = null) {
		parent::__construct($datatype, $tree);
		$this->currentLeaf = $currentLeafId;
	}

	public function html() {
		$this->decorate();
		return isset($this->currentLeaf) ? $this->branchHtml() : $this->treeHtml();
	}

	protected function branchHtml(){
		return isset($this->tree->leafs[$this->currentLeaf])
			? PXAdminAjaxTreeBranch::childrenHTML($this->tree->leafs[$this->currentLeaf]->children, $this)
			: '';
	}

	protected function treeHtml(){
		$caption = $this->htmlCaption();

		$html = '';
		$html .= join($this->blocks[self::BEFORE]);

		if (sizeof($this->tree->leafs[0]->children) > 0) {
			// Рекурсивно генерируем дерево
			$data = $this->showChildLinks ? ' data-cl="'.$this->childLinkVarName.'"' : '';
			$html .= '<div class="tree"' . $data . '>';
				$html .= $caption;
				$html .= join($this->blocks[self::BEFORE_CONTENT]);
				$html .= '<div class="tree__container">';
					$html .= PXAdminAjaxTreeBranch::childrenHTML($this->tree->leafs[0]->children, $this);
				$html .= '</div>';
				$html .= join($this->blocks[self::AFTER_CONTENT]);
			$html .= '</div>';
		} else {
			$html .= '<div class="tree">' . $caption . '<div class="empty">&#8212;</div></div>';
		}

		$html .= join($this->blocks[self::AFTER]);

		return $html;
	}

	public /* override */ function isOpenLeaf($id) {
		return in_array($this->datatype->id . $id, $this->leafsToOpen);
	}

	public /* override */ function setSelected($selectedSid) {
		$this->selectedSid = $selectedSid;

		// Добавляем в массив "расхлопывающихся" листьев всех предков выбранного
		$tmpId = $selectedSid;
		while (isset($this->tree->leafs[$tmpId]->parent)) {
			$tmpId = $this->tree->leafs[$tmpId]->parent;
			$this->leafsToOpen[] = $this->datatype->id . $tmpId;
		}

	}

	public function isLastLeaf($id) {
		if(!isset($this->tree->leafs[$id])){
			return false;
		}
		$leaf = $this->tree->leafs[$id];
		$pid = $leafs->parent;
		if(!isset($this->tree->leafs[$pid])){
			return false;
		}
		$parent = $this->tree->leafs[$pid];
		return end($parent->children) == $id;
	}


}
