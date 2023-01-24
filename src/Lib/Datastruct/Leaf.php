<?php

namespace PP\Lib\Datastruct;

/**
 * Class Leaf
 * @package PP\Lib\Datastruct
 */
class Leaf {
	public $children;

	/**
	 * @var int
	 */
	public $level;

	/**
	 * @var Tree
	 */
	public $tree;

	/**
	* Leaf constructor.
	*
	* @param $id
	* @param $title
	* @param $parentId
	* @param $content
	*/
	public function __construct(public $id, public $title, public $parent, public $content, Tree $tree) {
		$this->children = [];
		$this->level = 0;
		$this->tree = $tree;
	}

	public function getAncestors($andSelf = false) {
		return $this->tree->getAncestors($this->id, $andSelf);
	}

	public function parent($level = 1) {
		$ancestors = $this->getAncestors();

		if ($level < 0) {
			$level = -$level + 1;
			$ancestors = array_reverse($ancestors);
		}

		if (isset($ancestors[$level - 1])) {
			return $ancestors[$level - 1];
		} else {
			return null;
		}
	}

	public function getDescendants($level = null) {
		return $this->tree->getDescendants($this->id, $level);
	}

	public function createpath() {
		return createPathByParentId($this->tree, $this->id);
	}

	public function createpathWithoutRoot() {
		return createSomePathByParentId($this->tree, $this->id, 'pathname', '/', true, false);
	}

	/**
	 * Determines whether the current leaf is root
	 *
	 * @return bool
	 */
	public function isRoot() {
		return $this->id == $this->tree->getRoot()->id;
	}

	/**
	 * Removes itself from the tree
	 */
	public function tearOff() {
		unset($this->tree->leafs[$this->id]);
	}
}
