<?php

class PXAdminTreeObjects extends PXAdminObjects
{
	/**
  *
  * @param mixed $format
  * @param string $loadWhere
  * @param string $loadMethod
  * @param string $withoutExternals
  */
 public function __construct($format, protected $loadWhere = null, protected $loadMethod = null, protected $withoutExternals = false)
	{
		parent::__construct($format);

		$this->createSubWidget();
	}

	public function loadObjects()
	{
		$cacheDataKey = self::makeDataKey($this->format->id);
		if ($r = self::retrieveData($cacheDataKey)) {
			$this->tree = $r;
			$this->count = sizeof($this->tree->leafs) - 1;
			return;
		}

		$this->tree = match (strtolower((string) $this->loadMethod)) {
			'byparent' => $this->db->getObjectsByParent($this->format, null, $this->loadWhere, DB_SELECT_TREE | ($this->withoutExternals ? DB_SELECT_WITHOUT_EXTERNALS : 0)),
			'byidarray' => $this->db->getObjectsByIdArray($this->format, null, $this->loadWhere, DB_SELECT_TREE | ($this->withoutExternals ? DB_SELECT_WITHOUT_EXTERNALS : 0)),
			default => $this->db->getObjects($this->format, null, DB_SELECT_TREE | ($this->withoutExternals ? DB_SELECT_WITHOUT_EXTERNALS : 0)),
		};

		$this->db->filterByAccess('admin', $this->format, $this->tree);
		$this->count = sizeof($this->tree->leafs) - 1;

		self::storeData($cacheDataKey, $this->tree);
	}

	public function getControlParent()
	{
		return is_null($this->controlParent) ? $this->tree->getRoot()->id : $this->controlParent;
	}

	public function createSubWidget()
	{
		$this->loadObjects();
		$this->widget = new PXAdminTree($this->format, $this->tree);
		$this->widget->setOpenedLeafs($this->request->getLeafStatus());
		$this->widget->loadDropdownValues();
	}

	public function setSelected($selectedSid)
	{
		$this->widget->setSelected($selectedSid);
	}

	public function appendControl($name, $template)
	{
		// pass
	}

	public function has($id)
	{
		return isset($this->tree->leafs[$id]);
	}

	public function get($id)
	{
		if (!$this->has($id)) {
			FatalError('Can\'t find ' . $id);
		}

		return $this->tree->leafs[$id]->content;
	}

	public function getTree()
	{
		return $this->tree;
	}
}
