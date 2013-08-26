<?php
class PXPreviewEngine extends PXEngineIndex {

	// @overload
	protected $db = array('factory' => 'PXPreviewDB', 'helper' => true);

	private $path;

	public function __construct($path) {
		$this->path = $path;
		parent::__construct();
	}

	protected function initApplication() {
		parent::initApplication();
		$this->request->setPath($this->path);
	}

}
