<?php
class PXPreviewEngine extends PXEngineIndex {

	// @overload
	protected $db = array('factory' => 'PXPreviewDB', 'helper' => true);

	private $path;

	function __construct($path) {
		parent::__construct();
		$this->path = $path;
	}

	protected function initApplication() {
		parent::initApplication();
		$this->request->setPath($this->path);
	}
}
