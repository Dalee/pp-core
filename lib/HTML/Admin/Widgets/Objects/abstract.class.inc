<?php
abstract class PXAdminObjects extends PXAdminWidget implements IPXAdminObjects {
	protected $format;
	protected $isShowCaption = true;
	protected $count = 0;

	function __construct($format) {
		if(is_string($format)) {
			$format = PXRegistry::getTypes($format);
		}

		$this->format = $format;

		$this->app     = PXRegistry::getApp();
		$this->db      = PXRegistry::getDb();
		$this->request = PXRegistry::getRequest();
		$this->layout  = PXRegistry::getLayout();
	}

	abstract protected function createSubWidget();

	public
	function showCaption() {
		$this->isShowCaption = true;
	}

	public
	function hideCaption() {
		$this->isShowCaption = false;
	}

	public
	function showChildren($rVarName) {
		$cId  = $this->request->getVar($rVarName);

		$this->widget->setSelected($cId, $this->format->id);
		$this->widget->setShowChildObjects(true);
		$this->widget->setChildLink($rVarName);
	}

	public
	function addToParent($cellName) {
		parent::addToParent($cellName);

		$this->layoutCell = $cellName;

		$cell = explode('.', $cellName);
		$cell[2]++;
		$this->layoutButtonCell = implode('.', $cell);

		$this->_addControls();
	}

	private
	function _addControls() {
		$parent = $this->getControlParent();

		if(!PXRegistry::getUser()->can(array('write', 'add'), $this->format, array('parent' => $parent, 'id' => null))) {
			return;
		}

		$this->layout->appendControls($this->layoutButtonCell, $parent, array($this->format->id));

		// FIXME - need append, not assign
		$this->layout->assignContextMenu($this->layoutCell.'.CONTEXT', $parent, array($this->format->id));
	}

	protected
	function htmlCaption() {
		if($this->isShowCaption) {
			$caption = $this->format->title;

			if($this->count > 0) {
				$caption .= ' (' . $this->count . ')';
			}

			$this->widget->setCaption($caption);
		}
	}

	function html() {
		$this->htmlCaption();

		return $this->widget->html();
	}
}
?>