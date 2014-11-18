<?php
class NLLeaf {
	var $id;
	var $title;
	var $children;
	var $parent;
	var $content;
	var $level;
	var $tree;

	function NLLeaf($id, $title, $parentId, $content, $tree) {
		$this->id       = $id;
		$this->title    = $title;
		$this->children = array();
		$this->parent   = $parentId;
		$this->content  = $content;
		$this->level    = 0;
		$this->tree     = $tree;
	}

	function getAncestors($andSelf = false) {
		return $this->tree->GetAncestors($this->id, $andSelf);
	}

	function parent($level=1) {
		$ancestors = $this->getAncestors();

		if($level < 0) {
			$level = -$level + 1;
			$ancestors = array_reverse($ancestors);
		}

		if(isset($ancestors[$level - 1])) {
			return $ancestors[$level - 1];
		} else {
			return null;
		}
	}

	function getDescendants($level=null) {
		return $this->tree->getDescendants($this->id, $level);
	}

	function createpath() {
		return createPathByParentId($this->tree, $this->id);
	}

	function createpathWithoutRoot() {
		return createSomePathByParentId($this->tree, $this->id, 'pathname', '/', true, false);
	}

	function isRoot() {
		return $this->id == $this->tree->getRoot()->id;
	}

	function tearOff() {
		unset($this->tree->leafs[$this->id]);
	}
}

/**
 * Класс-репрезентация абстрактного дерева
 *
 * @author Michael Beloshapka <mb@dalee.ru>
 * @package ProximaPortal
 * @subpackage Datastruct
 */
class NLTree {
	var $leafs;
	var $levels;
	var $_idField;
	var $_parentField;
	var $_titleField;

	/**
	 * Конструктор класса
	 *
	 * @param Array   $table       таблица данных, из которых будет строиться таблица
	 * @param Integer $idField     название поля таблицы, в котором лежат id
	 * @param Integer $parentField название поля таблицы, в котором лежат id родителей
	 * @param Integer $titleField  название поля таблицы, в котором лежат названия объектов
	 */
	function NLTree($table, $idField = 'id', $parentField = 'parent', $titleField = 'title', $saveOrphans=false) {
		$this->_idField     = $idField;
		$this->_parentField = $parentField;
		$this->_titleField  = $titleField;
		$this->leafs        = array();
		$this->levels       = array();
		$this->leafs[0]     = new NLLeaf(0, "Root", NULL, array(), $this);

		$this->saveOrphans  = $saveOrphans;

		$this->_fillLeafs($table);
		$this->_fillLevel(0, 0);
	}

	function walk ($closure = false, $rootId = null) {

		if (is_null($rootId)) {
			$rootId = 0;
		}
		if (empty($this->leafs[$rootId])) {
			return trigger_error('Unexistent leaf id passed');
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

	function map ($closure = false, $rootId = null) {

		if (is_null($rootId)) {
			$rootId = 0;
		}
		if (empty($this->leafs[$rootId])) {
			return trigger_error('Unexistent leaf id passed');
		}

		$leafs = $this->leafs;
		$traversing = function ($id) use (&$leafs, $closure, &$traversing) {
			$leaf = $leafs[$id];

			$result = array();
			if (!empty($leaf->children)) {
				foreach ($leaf->children as $id => $child) {
					$result[] = $traversing($child);
				}
			}

			return $closure($leaf, $result);
		};

		$result = array();
		foreach ($leafs[$rootId]->children as $id => $child) {
			$result[] = $traversing($child);
		}

		return $result;
	}

	function getRoot() {
		return $this->leafs[0];
	}

	function _fillLeafs(&$table) {
		// Filling leafs of tree
		foreach ($table as $k=>$v) {
			$id     = $v[$this->_idField];
			$title  = $v[$this->_titleField];

			$parent = (isset($v[$this->_parentField])) ? $v[$this->_parentField] : 0; // For creating trees from plain
			$this->leafs[$id] = new NLLeaf($id, $title, $parent, $v, $this);
		}

		// Filling children attributes of leafs of tree
		foreach ($this->leafs as $k=>$v) {
			if (!is_null($v->parent) && isset($this->leafs[$v->parent])) {
				$this->leafs[$v->parent]->children[] = $k;

			} elseif ($v->id != 0 && $this->saveOrphans) {
				$this->leafs[0]->children[] = $k;
			}
		}
	}

	function _fillLevel($leafId, $level) {
		// Filling level array
		$this->levels[$level][] = $leafId;
		$this->leafs[$leafId]->level = $level;
		foreach ($this->leafs[$leafId]->children as $childId) {
			$this->_fillLevel($childId, $level+1);
		}
	}

	function getFullPath($id) {
		if (!isset($this->leafs[$id])) return array();
		$ret = array($id);
		while ($id != 0) {
			if (!isset($this->leafs[$id])){
				return array();
			}
			$id = $this->leafs[$id]->parent;
			if ($id != 0) $ret[] = $id;
		}
		return array_reverse($ret);
	}

	function getFullPathString($id, $varName = 'pathname', $omitFirst = true) {
		$pathArray  = $this->getFullPath($id);
		$pathString = NULL;
		if (is_array($pathArray) && count($pathArray) && isset($this->leafs[$pathArray[0]]->content[$varName])) {
			$pathString = '/';
			foreach ($pathArray as $k=>$v) {
				if ($omitFirst && $k == 0) continue;
				$pathString .= $this->leafs[$v]->content[$varName].'/';
			}
		}
		return $pathString;
	}

	function getIdArrayByPath($varName, $pathArray) {
		$idArray = array();
		$id      = 0;

		while (count($pathArray)) {
			$tmpFlag = 0;

			foreach ($this->leafs[$id]->children as $leafId) {
				if ($this->leafs[$leafId]->content[$varName] == $pathArray[0]) {
					$idArray[] = $leafId;
					$id        = $leafId;
					$tmpFlag   = 1;
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

	function GetPlainTree($restrictedId, $id = 0, $parent = NULL, $current = NULL, $level = 1, $prefix = '') {
		$t = array();

		foreach ($this->leafs[$id]->children as $child) {
			if ($child == $restrictedId) {
				continue;
			}

			if ($child != $current) {
				$t[$child] = $prefix.' '.$this->leafs[$child]->title;
				$t = $t + $this->GetPlainTree($restrictedId, $child, $parent, $current, $level+1, $prefix.'===');
			}
		}

		if ($id == 0) {
			$t[NULL] = "Корень";
		}

		return $t;
	}

	function IsAncestor($id, $testId) {
		if (isset($this->leafs[$id]->parent) && $this->leafs[$id]->parent) {
			if($this->leafs[$id]->parent == $testId) {
				return true;
			} else {
				return $this->IsAncestor($this->leafs[$id]->parent, $testId);
			}

		} else {
			return false;
		}
	}

	function GetDescendantsOrSelf($parents) {
		$retArray = array();
		$parents = array_flip($parents);

		foreach ($this->leafs as $leaf) {
			if(!isset($leaf->id)) {
				d2($leaf);
			}
			if (isset($parents[$leaf->id])) {
				$retArray = array_merge($retArray, $this->GetDescendants($leaf->id));
			}
		}

		return $retArray;
	}

	function GetDescendants($id, $level=null) {
		$retArray[] = $id;

		if(isset($this->leafs[$id]->children) && ($level === null || $this->leafs[$id]->level < $level)) {
			foreach ($this->leafs[$id]->children as $child) {
				$retArray = array_merge($retArray, $this->GetDescendants($child, $level));
			}
		}

		return $retArray;
	}

	function GetAncestors($id, $andSelf=false) {
		$ancestors = array();

		if(isset($this->leafs[$id]) && $this->leafs[$id]->parent !== null) {
			if($andSelf) {
				$ancestors = array($id, $this->leafs[$id]->parent);
			} else {
				$ancestors = array($this->leafs[$id]->parent);
			}

			$ancestors = array_merge($ancestors, $this->GetAncestors($this->leafs[$id]->parent));
		}

		return $ancestors;
	}

	function addLeaf($leaf) {
		$this->leafs[$leaf->id] = $leaf;
		$this->leafs[$leaf->parent]->children[] = $leaf->id;
	}

	function removeLeaf($leafid) {
		if ($leafid == 0) return;
		if (isset($this->leafs[$this->leafs[$leafid]->parent])) {
			if (($l = array_search($leafid, $this->leafs[$this->leafs[$leafid]->parent]->children)) !== false) {
				unset($this->leafs[$this->leafs[$leafid]->parent]->children[$l]);
			}
			$this->leafs[$this->leafs[$leafid]->parent]->children = array_values($this->leafs[$this->leafs[$leafid]->parent]->children);
		}
		unset($this->leafs[$leafid]);
	}

	function toTable() {
		$table = array();
		foreach ($this->leafs as $l) {
			if ($l->id == 0) continue;
			$object = $l->content;
			$object[$this->_idField] = $l->id;
			$object[$this->_parentField] = $l->parent;
			$object[$this->_titleField] = $l->title;
			$table[$l->id] = $object;
		}
		return $table;
	}
}

?>
