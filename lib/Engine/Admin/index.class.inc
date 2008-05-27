<?php
class PXEngineAdminIndex extends PXEngine {
	var $menu;

	function __construct() {
		parent::__construct();

		$types = PXRegistry::getApp()->types;
		PXRegistry::setLayout(new PXAdminHTMLLayout('index', $types));
	}

	function initModules() {
		$this->modules = PXRegistry::getApp()->getAvailableModules(PXRegistry::getUser()->level);
	}

	function initMenu() {
		$menuItems = array();
		foreach ($this->modules as $module) {
			if($module->hidetab) {
				continue;
			}

			$menuItems[$module->name] = $module->description;
		}

		$this->menu = $menuItems;
	}

	function showAuthForm() {
		$auth = $this->modules['auth']->getModule();
		$auth->adminIndex();
	}

	function fillLayout($area) {
		$layout = PXRegistry::getLayout();

		$layout->setLogoutForm('?area=exit');
		$layout->setMenu($this->menu, $area);
		$layout->setSimpleInnerLayout(array('25%', '75%'), array('100%', ''));

		$layout->setGetVarToSave('area', $area);
		$layout->setGetVarToSave('sid', PXRegistry::getRequest()->getSid());
	}

	function checkArea($area) {
		if (!isset($this->modules[$area])) {
			$layout = PXRegistry::getLayout();

			$layout->setSimpleInnerLayout(array('100%'), array('100%'));
			$layout->assignError('INNER.0.0', 'Некорректный параметр <em>area</em> = <em>'.strip_tags($area).'</em>');
			$layout->assignTitle('Некорректный параметр area');
			return false;
		}

		return true;
	}

	function runModules() {
		NLProfiler('RUN MODULES', 'BEGIN');

		$this->initMenu();

		if (!PXRegistry::getUser()->isAdmin()) {
			return $this->showAuthForm();
		}

		$area = PXRegistry::getRequest()->getArea(current(array_keys($this->menu)));
		$this->fillLayout($area);

		if($area == 'exit') {
			Finalize('action.phtml?area=auth&action=exit');
		}

		if(!$this->checkArea($area)) {
			return;
		}

		$instance = $this->modules[$area]->getModule()->adminIndex();

		NLProfiler('RUN MODULES', 'END');
	}

	function html() {
		$charset = PXRegistry::getApp()->GetProperty('OUTPUT_CHARSET', DEFAULT_CHARSET);

		PXRegistry::getDb()->close();
		PXRegistry::getLayout()->flush($charset);
	}
}
?>
