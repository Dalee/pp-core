<?php
class PXEngineAdminIndex extends PXEngineInterface {
	var $menu;

	function initLayout() {
		$this->layout = new PXAdminHTMLLayout('index', $this->app->types);
	}

	function initModules() {
		$this->modules = $this->app->getAvailableModules($this->user->level);
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
		$auth->adminIndex($this->app, $this->request, $this->user, $this->db, $this->layout);
	}

	function fillLayout($area) {
		$layout = $this->layout;

		$layout->setLogoutForm('?area=exit');
		$layout->setMenu($this->menu, $area);
		$layout->setSimpleInnerLayout(array('25%', '75%'), array('100%', ''));

		$layout->setGetVarToSave('area', $area);
		$layout->setGetVarToSave('sid', $this->request->getSid());
	}

	function checkArea($area) {
		if (!isset($this->modules[$area])) {
			$layout = $this->layout;

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

		if (!$this->user->isAdmin()) {
			return $this->showAuthForm();
		}

		$area = $this->request->getArea(current(array_keys($this->menu)));
		$this->fillLayout($area);

		if($area == 'exit') {
			Finalize('action.phtml?area=auth&action=exit');
		}

		if(!$this->checkArea($area)) {
			return;
		}

        $instance = $this->modules[$area]->getModule();
		$instance->adminIndex($this->app, $this->request, $this->user, $this->db, $this->layout);

		NLProfiler('RUN MODULES', 'END');
	}

	function profilerStart() {
	}

	function profilerStop() {
	}

	function html() {
		$this->db->close();
		$this->layout->flush($this->app->GetProperty('OUTPUT_CHARSET', DEFAULT_CHARSET));
	}
}
?>
