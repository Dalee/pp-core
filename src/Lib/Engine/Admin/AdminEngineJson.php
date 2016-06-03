<?php

namespace PP\Lib\Engine\Admin;

use PXModule;

class AdminEngineJson extends \PXEngineJSON {

	/**
	 *
	 */
	function initModules() {
		$this->area = $this->request->getArea();
		$this->modules = AbstractAdminEngine::getModule($this->app, $this->area);
		$this->checkArea($this->area);
	}

	/**
	 * @param PXModule $module
	 * @return string|null
	 */
	function getJson(PXModule $module) {
		return $module->adminJson();
	}

	/**
	 * @return int
	 */
	public function engineClass() {
		return PX_ENGINE_ADMIN;
	}
}
