<?php

class PXAdminTreeObjects extends PXAdminObjects {
	protected $withoutExternals;
	protected $loadWhere = null;
	protected $loadMethod = null;

	/**
	 *
	 * @param mixed $format
	 * @param string $where
	 * @param string $loadMethod
	 * @param string $withoutExternals
	 */
	function __construct($format, $where = null, $loadMethod = null, $withoutExternals = false) {
		$this->loadWhere = $where;
		$this->loadMethod = $loadMethod;
		$this->withoutExternals = $withoutExternals;

		parent::__construct($format);

		$this->createSubWidget();
	}

	function loadObjects() {
		$cacheDataKey = self::makeDataKey($this->format->id);
		if ($r = self::retrieveData($cacheDataKey)) {
			$this->tree = $r;
			$this->count = sizeof($this->tree->leafs) - 1;
			return;
		}

		switch (strtolower($this->loadMethod)) {
			case 'byparent':
				$this->tree = $this->db->getObjectsByParent($this->format, null, $this->loadWhere, DB_SELECT_TREE | ($this->withoutExternals ? DB_SELECT_WITHOUT_EXTERNALS : 0) );
				break;
			case 'byidarray':
				$this->tree = $this->db->getObjectsByIdArray($this->format, null, $this->loadWhere, DB_SELECT_TREE | ($this->withoutExternals ? DB_SELECT_WITHOUT_EXTERNALS : 0) );
				break;
			default:
				$this->tree = $this->db->getObjects($this->format, null, DB_SELECT_TREE | ($this->withoutExternals ? DB_SELECT_WITHOUT_EXTERNALS : 0) );
				break;
		}

		$this->db->filterByAccess('admin', $this->format, $this->tree);
		$this->count = sizeof($this->tree->leafs) - 1;

		self::storeData($cacheDataKey, $this->tree);
	}

	function getControlParent() {
		return is_null($this->controlParent) ? $this->tree->getRoot()->id : $this->controlParent;
	}

	function createSubWidget() {
		$this->loadObjects();
		$this->widget = new PXAdminTree($this->format, $this->tree);
		$this->widget->setOpenedLeafs($this->request->getLeafStatus());
		$this->widget->loadDropdownValues($this->db);
	}

	function setSelected($selectedSid) {
		$this->widget->setSelected($selectedSid);
	}

	function appendControl($name, $template) {
		// pass
	}

	function has($id) {
		return isset($this->tree->leafs[$id]);
	}

	function get($id) {
		if(!$this->has($id)) {
			FatalError('Can\'t find '.$id);
		}

		return $this->tree->leafs[$id]->content;
	}

	function getTree() {
		return $this->tree;
	}
}
?>
