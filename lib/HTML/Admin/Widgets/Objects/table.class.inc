<?php
class PXAdminTableObjects extends PXAdminObjects  {
	protected $objects = array();
	protected $additionalArgs = array();

	function __construct($format, $where=NULL, $loadMethod='') {
		parent::__construct($format);
		$this->where  = $where;
		$this->loadMethod = $loadMethod;
		$this->getAdditionalParams(func_get_args());

		$this->createSubWidget();
	}

	function getControlParent() {
		if(!is_null($this->controlParent)) {
			$parent = $this->controlParent;

		} else if(!is_null($this->format->parent)) {
			$parent = current(GetColFromTable($this->objects, 'parent'));

		} else {
			$parent = null;
		}

		return $parent;
	}

	private
	function getAdditionalParams($arguments) {
		$this->additionalArgs = array_slice($arguments, 3);
	}

	function getWhere() {
		return $this->where;
	}


	private
	function loadObjects($where=NULL, $mode=DB_SELECT_TABLE, $limit=null, $offset=null, $order=NULL) {
		$args = array(
			$this->format,
			NULL
		);

		$method = 'getObjects';
		if(strlen($this->loadMethod)) {
			$method .= $this->loadMethod;
			$args[] = $where;

		} elseif(strlen($where)) {
			$method .= 'ByWhere';
			$args[] = $where;
		}

		if(sizeof($this->additionalArgs) > 0) {
			$args = array_merge($args, $this->additionalArgs);
		}

		if(!is_null($limit) && !is_null($offset)) {
			$method .= 'Limited';
			$args[] = $limit;
			$args[] = $offset;
		}

		$args[] = $mode;
		$args[] = $order;

		if(!method_exists($this->db, $method)) {
			FatalError('Undefined method '.$method.' in database');
		}

		return call_user_func_array(array($this->db, $method), $args);
	}

	protected
	function createSubWidget() {
		$this->count       =& $this->loadObjects($this->getWhere(), DB_SELECT_COUNT);

		$onPage      =  $this->app->getProperty('CHILDREN_ON_PAGE', 20);
		$currentPage =  $this->request->getVar($this->format->id.'_page', 1);

		$this->layout->setGetVarToSave($this->format->id.'_page', $currentPage);
		$this->layout->setGetVarToSave($this->format->id.'_view', 'plain');

		$currentPage  = $this->request->getVar($this->format->id.'_page', 1);
		$currentOrder = $this->request->getOrderVar($this->format->id, $this->format->order, $this->format->fields);

		$this->layout->setGetVarToSave($this->format->id.'_order', $currentOrder);

		$currentPage =  $currentPage > ceil($this->count/$onPage) ? ceil($this->count/$onPage) : $currentPage;

		$this->objects     =& $this->loadObjects($this->getWhere(), DB_SELECT_TABLE, $onPage, $onPage*($currentPage-1), $currentOrder);
		$this->db->filterByAccess('admin', $this->format, $this->objects);

		$this->pager = new PxAdminPager($currentPage, $onPage, $this->count, $this->format, $this->layout->getData);

		$this->widget = new PXAdminTable($this->objects, $this->format, $this->layout->getData);
		$this->widget->setPosition($this->pager->getPosition());
		$this->widget->loadDropdownValues($this->db);

		$this->_setParentPathname();
	}

	private
	function _setParentPathname() {
		if(!sizeof($this->objects)) {
			return;
		}

		$pathnames = $this->_loadParentPathnames($this->format, $this->objects);

		if(is_array($pathnames)) {
			$this->widget->setParentPathnames($pathnames);
		}
	}

	function appendControl($name, $template) {
		$this->widget->appendControl($name, $template);
	}

	private
	function _loadParentPathnames($format, $objects) {
		if(!isset($format->fields['pathname'])) {
			return;
		}

		if(!isset($format->parent)) {
			return;
		}

		$parentsIds = GetColFromTable($objects, 'parent');

		$pathnames = array();

		if($format->parent == 'struct') {
			$tree = $this->db->getObjects($format->parentType(), null, DB_SELECT_TREE);
			foreach($parentsIds as $p) {
				$pathnames[$p] = isset($tree->leafs[$p]) ? $tree->leafs[$p]->createpath() : '';
			}

		} else {
			$parents = $this->db->getObjectsByIdArray($format->parentType(), null, $parentsIds);

			$parentPathnames = $this->_loadParentPathnames($format->parentType(), $parents);

			foreach($parents as $p) {
				$pathnames[$p['id']] = $parentPathnames[$p['parent']].$p['pathname'];
			}
		}

		return $pathnames;
	}


	function has($id) {
		return isset($this->objects[$id]);
	}

	function get($id) {
		if(!$this->has($id)) {
			FatalError('Can\'t find '.$id);
		}

		return $this->objects[$id];
	}

	function html() {
		return parent::html().$this->pager->html();
	}
}
?>