<?php

namespace PP\Lib\Datastruct;

/**
 * Class Tree
 * @package PP\Lib\Datastruct
 * @todo refactor constructor method into ::fromTable static method
 * @todo add chaining support
 * @todo access level for "leafs" and "levels" should be protected
 */
class Tree {
	/** @var Leaf[] */
	public $leafs;

	/** @var array */
	public $levels;

	// TODO: Database access to this vars
	public $_idField;

	public $_parentField;

	public $_titleField;

	/**
	 * Tree constructor.
	 *
	 * @param array $table
	 * @param string $idField
	 * @param string $parentField
	 * @param string $titleField
	 * @param bool $saveOrphans
	 */
	public function __construct(
		$table,
		$idField = 'id',
		$parentField = 'parent',
		$titleField = 'title',
		$saveOrphans = false
	) {
		$this->_idField = $idField;
		$this->_parentField = $parentField;
		$this->_titleField = $titleField;

		$this->leafs = [];
		$this->levels = [];
		$this->leafs[0] = new Leaf(0, 'Root', null, [], $this);

		$this->saveOrphans = $saveOrphans;

		$this->fillLeafs($table);
		$this->fillLevel(0, 0);
	}

	/**
	 * @param boolean $saveOrphans
	 */
	public function setSaveOrphans($saveOrphans) {
		$this->saveOrphans = !!$saveOrphans;
	}

	/**
	 * Recursively collects all objects of allowed types within
	 * children
	 *
	 * @param integer $structId
	 * @param array $allowedTypes
	 * @return array
	 */
	public function recursiveChildren($structId, array $allowedTypes = []) {
		$childrenIds = $this->leafs[$structId]->children;
		if (empty($childrenIds)) {
			return [];
		}

		$result = [$structId];

		foreach ($childrenIds as $childId) {
			$item = $this->leafs[$childId]->content;
			if (!empty($allowedTypes) && !in_array($item['type'], $allowedTypes)) {
				continue;
			}

			$result[] = $childId;
			$tmp = $this->recursiveChildren($childId, $allowedTypes);
			if (!empty($tmp)) {
				$result = array_merge($result, $tmp);
			}
		}

		return $result;
	}

	protected function fillLeafs($table) {
		// Filling leafs of tree
		foreach ($table as $k => $v) {
			$id = $v[$this->_idField];
			$title = $v[$this->_titleField];

			// For creating trees from plain
			$parent = (isset($v[$this->_parentField])) ? $v[$this->_parentField] : 0;
			$this->leafs[$id] = new Leaf($id, $title, $parent, $v, $this);
		}

		// Filling children attributes of leafs of tree
		$toUnset = [];

		foreach ($this->leafs as $k => $v) {
			if (!is_null($v->parent) && isset($this->leafs[$v->parent])) {
				$this->leafs[$v->parent]->children[] = $k;

			} elseif ($v->id != 0 && $this->saveOrphans) {
				$this->leafs[0]->children[] = $k;
			} elseif ($v->id != 0 && !$this->saveOrphans) {
				$toUnset[] = $k;
			}
		}

		foreach ($toUnset as $key) {
			unset($this->leafs[$key]);
		}
	}

	protected function fillLevel($leafId, $level) {
		// Filling level array
		$this->levels[$level][] = $leafId;
		$this->leafs[$leafId]->level = $level;

		foreach ($this->leafs[$leafId]->children as $childId) {
			$this->fillLevel($childId, $level + 1);
		}
	}

	public function walk(callable $closure, $rootId = null) {
		$rootId = ($rootId) ?: 0;

		if (empty($this->leafs[$rootId])) {
			throw new \Exception("Absent rootId: {$rootId}");
		}

		$leafs = $this->leafs;
		$traversing = function ($id) use (&$leafs, $closure, &$traversing) {
			$leaf = $leafs[$id];
			$closure($leaf);

			if (!empty($leaf->children)) {
				foreach ($leaf->children as $id => $child) {
					$traversing($child);
				}
			}
		};

		return $traversing($rootId);
	}

	public function map(callable $closure, $rootId = null) {
		$rootId = ($rootId) ?: 0;

		if (empty($this->leafs[$rootId])) {
			throw new \Exception("Absent rootId: {$rootId}");
		}

		$leafs = $this->leafs;
		$traversing = function ($id) use (&$leafs, $closure, &$traversing) {
			$leaf = $leafs[$id];
			$result = [];

			if (!empty($leaf->children)) {
				foreach ($leaf->children as $id => $child) {
					$result[] = $traversing($child);
				}
			}

			return $closure($leaf, $result);
		};

		$result = [];
		foreach ($leafs[$rootId]->children as $id => $child) {
			$result[] = $traversing($child);
		}

		return $result;
	}

	/**
	 * Gets tree's root leaf
	 *
	 * @return Leaf
	 */
	public function getRoot() {
		return $this->leafs[0];
	}

	public function getFullPath($id) {
		if (!isset($this->leafs[$id])) {
			return [];
		}

		$ret = [$id];
		while ($id != 0) {
			if (!isset($this->leafs[$id])) {
				return [];
			}
			$id = $this->leafs[$id]->parent;
			if ($id != 0) $ret[] = $id;
		}
		return array_reverse($ret);
	}

	public function getFullPathString($id, $varName = 'pathname', $omitFirst = true) {
		$pathArray = $this->getFullPath($id);
		$pathString = NULL;
		if (is_array($pathArray) && count($pathArray) && isset($this->leafs[$pathArray[0]]->content[$varName])) {
			$pathString = '/';
			foreach ($pathArray as $k => $v) {
				if ($omitFirst && $k == 0) {
					continue;
				}
				$pathString .= $this->leafs[$v]->content[$varName] . '/';
			}
		}
		return $pathString;
	}

	public function getIdArrayByPath($varName, $pathArray) {
		$idArray = [];
		$id = 0;

		while (count($pathArray)) {
			$tmpFlag = 0;

			foreach ($this->leafs[$id]->children as $leafId) {
				if ($this->leafs[$leafId]->content[$varName] == $pathArray[0]) {
					$idArray[] = $leafId;
					$id = $leafId;
					$tmpFlag = 1;
				}
			}

			if (!$tmpFlag) {
				$idArray[] = -1;
				return $idArray; // CHECK ME
			}

			array_shift($pathArray);
		}

		return $idArray;
	}

	public function getPlainTree(
		$restrictedId,
		$id = 0,
		$parent = NULL,
		$current = NULL,
		$level = 1,
		$prefix = ''
	) {
		$t = [];

		foreach ($this->leafs[$id]->children as $child) {
			if ($child == $restrictedId) {
				continue;
			}

			if ($child != $current) {
				$t[$child] = $prefix . ' ' . $this->leafs[$child]->title;
				$t = $t + $this->getPlainTree($restrictedId, $child, $parent, $current, $level + 1, $prefix . '===');
			}
		}

		if ($id == 0) {
			$t[NULL] = 'Корень';
		}

		return $t;
	}

	public function isAncestor($id, $testId) {
		if (isset($this->leafs[$id]->parent) && $this->leafs[$id]->parent) {
			if ($this->leafs[$id]->parent == $testId) {
				return true;
			} else {
				return $this->isAncestor($this->leafs[$id]->parent, $testId);
			}

		} else {
			return false;
		}
	}

	public function getDescendantsOrSelf($parents) {
		$retArray = [];
		$parents = array_flip($parents);

		foreach ($this->leafs as $leaf) {
			if (isset($parents[$leaf->id])) {
				$retArray = array_merge($retArray, $this->getDescendants($leaf->id));
			}
		}

		return $retArray;
	}

	public function getDescendants($id, $level = null) {
		$retArray[] = $id;

		if (isset($this->leafs[$id]->children) && ($level === null || $this->leafs[$id]->level < $level)) {
			foreach ($this->leafs[$id]->children as $child) {
				$retArray = array_merge($retArray, $this->getDescendants($child, $level));
			}
		}

		return $retArray;
	}

	public function getAncestors($id, $andSelf = false) {
		$ancestors = [];

		if (isset($this->leafs[$id]) && $this->leafs[$id]->parent !== null) {
			if ($andSelf) {
				$ancestors = array($id, $this->leafs[$id]->parent);
			} else {
				$ancestors = array($this->leafs[$id]->parent);
			}

			$ancestors = array_merge($ancestors, $this->getAncestors($this->leafs[$id]->parent));
		}

		return $ancestors;
	}

	public function addLeaf(Leaf $leaf) {
		$parentId = $leaf->parent;
		$parentId = (is_null($parentId)) ? 0 : $parentId;
		$parentExists = isset($this->leafs[$parentId]);

		if ($parentExists || $this->saveOrphans) {
			$this->leafs[$leaf->id] = $leaf;

			if ($parentExists) {
				$this->leafs[$parentId]->children[] = $leaf->id;
			}
		}
	}

	/**
	 * Removes leaf by id from the tree
	 *
	 * @param int $leafId
	 */
	public function removeLeaf($leafId) {
		if ($leafId == 0) {
			return;
		}

		if (isset($this->leafs[$this->leafs[$leafId]->parent])) {
			if (($l = array_search($leafId, $this->leafs[$this->leafs[$leafId]->parent]->children)) !== false) {
				unset($this->leafs[$this->leafs[$leafId]->parent]->children[$l]);
			}
			$this->leafs[$this->leafs[$leafId]->parent]->children = array_values($this->leafs[$this->leafs[$leafId]->parent]->children);
		}

		unset($this->leafs[$leafId]);
	}

	public function toTable() {
		$table = [];
		foreach ($this->leafs as $l) {
			if ($l->id == 0) {
				continue;
			}

			$object = $l->content;
			$object[$this->_idField] = $l->id;
			$object[$this->_parentField] = $l->parent;
			$object[$this->_titleField] = $l->title;
			$table[$l->id] = $object;
		}

		return $table;
	}
}
