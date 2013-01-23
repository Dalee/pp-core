<?php
class PXAdminAjaxTreeObjects extends PXAdminTreeObjects {
	
	protected $currentBranch;
	
	public function __construct($format, $where=NULL, $loadMethod='', $withoutExternals = false, $currentBranch = null) {
		$this->currentBranch = $currentBranch;
		parent::__construct($format, $where, $loadMethod, $withoutExternals);
	}
	
	public function createSubWidget() {

		$this->loadObjects();
		
		$this->widget = new PXAdminAjaxTree($this->format, $this->tree, $this->currentBranch);

		$this->widget->setOpenedLeafs($this->request->getLeafStatus());
		$this->widget->loadDropdownValues($this->db);
	}
	
}