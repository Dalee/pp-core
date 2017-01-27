<?php

namespace PP\Lib\Engine\Admin;

/**
 * Class AdminEnginePopup
 * @package PP\Lib\Engine\Admin
 */
class AdminEnginePopup extends AdminEngineIndex {

	protected $outerLayout = 'popup';
	protected $templateMainArea = 'OUTER.CONTENT';

	function initModules() {
		$this->area = $this->request->getArea();
		$this->modules = $this->getModule($this->app, $this->area);
	}

	function fillLayout($area = null) {
		$this->layout->assignFlashes();
		$this->layout->setGetVarToSave('area', $this->area);
	}

	function runModules() {
		if (!$this->hasAdminModules()) {
			$this->layout->assignError($this->templateMainArea, 'Нет доступа');
			return;
		}

		if (!$this->checkArea($this->area)) {
			return;
		}

		$this->fillLayout();

		$instance = $this->modules[$this->area]->getModule();

		$this->layout->append($this->templateMainArea, $instance->adminPopup());
	}
}
