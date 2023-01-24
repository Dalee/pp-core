<?php

class PXPreviewEngine extends PXEngineIndex {

	// @overload
	protected $db = ['factory' => 'PXPreviewDB', 'helper' => true];

	public function __construct(private $path) {
		parent::__construct();
	}

	protected function initApplication() {
		parent::initApplication();
		$this->request->setPath($this->path);
	}

}
