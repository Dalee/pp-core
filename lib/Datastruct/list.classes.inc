<?php
class NLListItem {
	var $id;
	var $title;
	var $content;

	function NLListItem($id, $title, $content) {
		$this->id       = $id;
		$this->title    = $title;
		$this->content  = $content;
	}

}

/*
 * NLList - representative of list data-structure;
 */
class NLList {
	var $items;

	function NLList($table) {
		// Constructor
		$this->FillItems($table);
	}

	function FillItems($table) {
		// Filling leafs of tree
		foreach ($table as $k=>$v) {
			$id     = $v['id'];
			$title  = $v['title'];
			unset($v['id'], $v['title']);
			$this->items[$id] = new NLListItem($id, $title, $v);
		}
	}

}

?>
