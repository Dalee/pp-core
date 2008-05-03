<?php
class PXEngineAdminIndex extends PXEngineInterface {
	var $menu;

	function __construct() {
		parent::__construct();

		$types = PXEngineObjects::getApp()->types;
		PXEngineObjects::setLayout(new PXAdminHTMLLayout('index', $types));
	}

	function initModules() {
		$this->modules = PXEngineObjects::getApp()->getAvailableModules(PXEngineObjects::getUser()->level);
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
		$auth->adminIndex(PXEngineObjects::getApp(), PXEngineObjects::getRequest(), PXEngineObjects::getUser(), PXEngineObjects::getDb(), PXEngineObjects::getLayout());
	}

	function fillLayout($area) {
		$layout = PXEngineObjects::getLayout();

		$layout->setLogoutForm('?area=exit');
		$layout->setMenu($this->menu, $area);
		$layout->setSimpleInnerLayout(array('25%', '75%'), array('100%', ''));

		$layout->setGetVarToSave('area', $area);
		$layout->setGetVarToSave('sid', PXEngineObjects::getRequest()->getSid());
	}

	function checkArea($area) {
		if (!isset($this->modules[$area])) {
			$layout = PXEngineObjects::getLayout();

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

		if (!PXEngineObjects::getUser()->isAdmin()) {
			return $this->showAuthForm();
		}

		$area = PXEngineObjects::getRequest()->getArea(current(array_keys($this->menu)));
		$this->fillLayout($area);

		if($area == 'exit') {
			Finalize('action.phtml?area=auth&action=exit');
		}

		if(!$this->checkArea($area)) {
			return;
		}

		$instance = $this->modules[$area]->getModule()->indexAdmin();
		//$instance->adminIndex(PXEngineObjects::getApp(), PXEngineObjects::getRequest(), PXEngineObjects::getUser(), PXEngineObjects::getDb(), PXEngineObjects::getLayout());

		NLProfiler('RUN MODULES', 'END');
	}

	function html() {
		$charset = PXEngineObjects::getApp()->GetProperty('OUTPUT_CHARSET', DEFAULT_CHARSET);

		PXEngineObjects::getDb()->close();
		PXEngineObjects::getLayout()->flush($charset);
	}
}
?>
