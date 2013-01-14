<?php
class PXEngineAdminIndex extends PXEngineAdmin {
	var $menu;

	var $layout                 = array('factory' => 'PXAdminHTMLLayout', 'helper' => true);
	var $outerLayout            = 'index';
	protected $templateMainArea = 'INNER.0.0';

	function initLayout($klass){
		$this->layout = new $klass($this->outerLayout, $this->app->types);
	}

	function initModules() {
		$this->modules = $this->app->getAvailableModules();
	}

	function initMenu() {
		$menuItems = array();

		foreach ($this->modules as $module) {
			// check modules acl rules
			if (PXRegistry::getUser()->can('viewmenu', $module)) {
				$menuItems[$module->name] = $module->description == '' || $module->description == PXModuleDescription::EMPTY_DESCRIPTION ? $module->name : $module->description;
			}
		}

		$this->menu = $menuItems;
	}

	function showAuthForm() {
		if(!isset($this->modules[$this->authArea])) {
			FatalError('Undefined auth module or you forget insert "allo" for "admin" auth module in acl_objects');
		}

		$auth = $this->modules[$this->authArea]->getModule();
		$auth->adminIndex();
	}

	function fillLayout($area) {
		$this->layout->setLogoutForm('?area=exit');
		$this->layout->setMenu($this->menu, $area, 'area', false);

		$this->layout->setTwoColumns();

		$this->layout->setGetVarToSave('area', $area);
		$this->layout->setGetVarToSave('sid', $this->request->getSid());
	}

	function checkArea($area) {
		if (!isset($this->modules[$area])) {
			$this->layout->setOneColumn();
			$this->layout->assignError($this->templateMainArea, 'Некорректный параметр <em>area</em> = <em>' . strip_tags($area) . '</em>');
			$this->layout->assignTitle('Некорректный параметр area');
			return false;
		}

		return true;
	}

	function runModules() {
		PXProfiler::begin('RUN MODULES');

		$this->initMenu();

		if (!$this->hasAdminModules()) {
			return $this->showAuthForm();
		}

		$this->area = $this->request->getArea(current(array_keys($this->menu)));
		$this->fillLayout($this->area);

		if($this->area == 'exit') {
			$response =& PXResponse::getInstance();
			$response->redirect(sprintf('action.phtml?area=%s&action=exit', $this->authArea));
		}

		if(!$this->checkArea($this->area)) {
			return;
		}

		$instance = $this->modules[$this->area]->getModule()->adminIndex();

		PXProfiler::end();
	}

	function html() {
		$response =& PXResponse::getInstance();
		$response->dontCache();

		$charset = $this->app->GetProperty('OUTPUT_CHARSET', DEFAULT_CHARSET);

		PXRegistry::getDb()->close();
		$this->layout->flush($charset);
	}
}
?>
