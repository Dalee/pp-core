<?php
class PXAdminAjaxTree extends PXAdminTree {
	protected $currentLeaf;
	/**
	 * Конструктор класса PXAdminAjaxTree.
	 * Инициализирует поля класса. необходимые для построения дерева.
	 */
	public function __construct(&$datatype, &$tree, $currentLeafId = null) {
		parent::__construnct($datatype, $tree);
		$this->currentLeaf = $currentLeafId;
	}

	public function html() {
		return isset($this->currentLeafId) ? $this->branchHtml() : $this->treeHtml();
	}
	
	protected function branchHtml(){
		return isset($this->tree->leafs[$this->currentLeafId])
			? PXAdminAjaxTreeBranch::childrenHTML($this->tree->leafs[$this->currentLeafId]->children, $this)
			: '';
	}
	
	protected function treeHtml(){
		$caption = $this->htmlCaption();

		if(sizeof($this->tree->leafs[0]->children) > 0) {
			return '<div class="tree">' . $caption . PXAdminAjaxTreeBranch::childrenHTML($this->tree->leafs[0]->children, $this) . '</div>';
		} else {
			return '<div class="tree">' . $caption . '<div class="empty">&#8212;</div></div>';
		}
	}

}
?>