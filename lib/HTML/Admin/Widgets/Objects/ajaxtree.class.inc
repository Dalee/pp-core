<?php
class PXAdminAjaxTreeObjects extends PXAdminTreeObjects {
	
	public function __construct($format, $where=NULL, $loadMethod='', $withoutExternals = false, protected $currentBranch = null) {
		parent::__construct($format, $where, $loadMethod, $withoutExternals);
	}
	
	public function createSubWidget() {

		$this->loadObjects();
		
		$this->widget = new PXAdminAjaxTree($this->format, $this->tree, $this->currentBranch);

		$this->widget->setOpenedLeafs($this->request->getLeafStatus());
		$this->widget->loadDropdownValues();
	}
	
}