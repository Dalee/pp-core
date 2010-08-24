<?php
class PXAdminTableObjects extends PXAdminWidget {
	private $format;

	function __construct($format, $where=NULL) {
		if(is_string($format)) {
			$format = PXRegistry::getTypes($format);
		}

		$this->format = $format;
		$this->where  = $where;

		$this->app     = PXRegistry::getApp();
		$this->request = PXRegistry::getRequest();
		$this->layout  = PXRegistry::getLayout();

		$this->createTable();
	}

	function getWhere() {
		return $this->where;
	}

	private
	function loadObjects($where=NULL, $mode=DB_SELECT_TABLE, $limit=null, $offset=null, $order=NULL) {
		$db = PXRegistry::getDb();

		if(is_null($limit) || is_null($offset)) {
			if(is_null($where)) {
				$objects = $db->getObjects($this->format, NULL, $mode, $order);
			} else {
				$objects = $db->getObjectsByWhere($this->format, NULL, $where, $mode, $order);
			}

		} else {
			if(is_null($where)) {
				$objects = $db->getObjectsLimited($this->format, NULL, $limit, $offset, $mode, $order);
			} else {
				$objects = $db->getObjectsByWhereLimited($this->format, NULL, $where, $limit, $offset, $mode, $order);
			}
		}

		return $objects;
	}

	private
	function createTable() {
		$count       =& $this->loadObjects($this->getWhere(), DB_SELECT_COUNT);

		$onPage      =  $this->app->getProperty('CHILDREN_ON_PAGE', 20);
		$currentPage =  $this->request->getVar($this->format->id.'_page', 1);

		$this->layout->setGetVarToSave($this->format->id.'_page', $currentPage);

		$currentPage  = $this->request->getVar($this->format->id.'_page', 1);
		$currentOrder = $this->request->getOrderVar($this->format->id, $this->format->order, $this->format->fields);

		$this->layout->setGetVarToSave($this->format->id.'_order', $currentOrder);

		$currentPage =  $currentPage > ceil($count/$onPage) ? ceil($count/$onPage) : $currentPage;

		$objects     =& $this->loadObjects($this->getWhere(), DB_SELECT_TABLE, $onPage, $onPage*($currentPage-1), $currentOrder, $this->format);

		$this->table = new PXAdminTable($objects, $this->format, $this->layout->getData);
		$this->table->setPager($currentPage, $onPage, $count);

		$this->table->loadDropdownValues(PXRegistry::getDb());
	}

	function showChildren($rVarName) {
		$cId  = $this->request->getVar($rVarName);

		$this->table->setSelected($cId, $this->format->id);
		$this->table->setShowChildObjects(true);
		$this->table->setChildLink($rVarName);
	}

	public
	function addToParent($cellName) {
		parent::addToParent($cellName);

		$this->layoutCell = $cellName;

		$cell = explode('.', $cellName);
		$cell[2]++;
		$this->layoutButtonCell = implode('.', $cell);
	}

	function html() {
		$this->layout->assignContextMenu($this->layoutCell.'.CONTEXT', 0, array($this->format->id));
		$this->layout->assignControls($this->layoutButtonCell, 0, array($this->format->id));

		return $this->table->html();
	}
}
?>