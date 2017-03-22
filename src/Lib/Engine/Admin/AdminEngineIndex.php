<?php

namespace PP\Lib\Engine\Admin;

use Symfony\Component\DependencyInjection\ContainerAwareInterface;
use PXModuleDescription;
use PXResponse;
use PP\Lib\Html\Layout\AdminHtmlLayout;

/**
 * Class AdminEngineIndex.
 *
 * @package PP\Lib\Engine\Admin
 */
class AdminEngineIndex extends AbstractAdminEngine {

	/** @var AdminHtmlLayout */
	protected $layout = ['factory' => 'PP\Lib\Html\Layout\AdminHtmlLayout', 'helper' => true];
	protected $menu;
	protected $outerLayout = 'index';
	protected $templateMainArea = 'INNER.0.0';

	function initLayout($klass) {
		$this->layout = new $klass($this->outerLayout, $this->app->types);
	}

	function initModules() {
		$this->modules = $this->app->getAvailableModules();
	}

	function initMenu() {
		$menuItems = array();

		foreach ($this->modules as $module) {
			// check modules acl rules
			if ($this->user->can('viewmenu', $module)) {
				if ($module->getDescription() == '' || $module->getDescription() == PXModuleDescription::EMPTY_DESCRIPTION) {
					$menuItems[$module->getName()] = $module->getName();
				} else {
					$menuItems[$module->getName()] = $module->getDescription();
				}
			}
		}

		$this->menu = $menuItems;
	}

	function showAuthForm() {
		if (!isset($this->modules[$this->authArea])) {
			FatalError('Undefined auth module or you forget insert "allo" for "admin" auth module in acl_objects');
		}

		$auth = $this->modules[$this->authArea]->getModule();
		$auth->adminIndex();
	}

	function fillLayout() {
		$this->layout->assignFlashes();
		$this->layout->setLogoutForm('?area=exit');
		$this->layout->setMenu($this->menu, $this->area, 'area', false);

		$this->layout->setTwoColumns();

		$this->layout->setGetVarToSave('area', $this->area);
		$this->layout->setGetVarToSave('sid', $this->request->getSid());
	}

	protected function checkArea($area) {
		if (!isset($this->modules[$area])) {
			$this->layout->setOneColumn();
			$this->layout->assignError($this->templateMainArea, 'Некорректный параметр <em>area</em> = <em>' . strip_tags($area) . '</em>');
			$this->layout->assignTitle('Некорректный параметр area');
			return false;
		}

		return true;
	}

	public function runModules() {
		$this->initMenu();

		if (!$this->hasAdminModules()) {
			$this->showAuthForm();
			return;
		}

		$this->area = $this->request->getArea(current(array_keys($this->menu)));
		$this->fillLayout();

		if ($this->area == 'exit') {
			$this->session->invalidate(1);

			$response = PXResponse::getInstance();
			$response->redirect(sprintf('action.phtml?area=%s&action=exit', $this->authArea));
		}

		if (!$this->checkArea($this->area)) {
			return;
		}

		$instance = $this->modules[$this->area]->getModule();
		if ($instance instanceof ContainerAwareInterface) {
			$instance->setContainer($this->container);
		}
		$instance->adminIndex();
	}

	public function html() {
		$response = PXResponse::getInstance();
		$response->dontCache();

		$charset = $this->app->getProperty('OUTPUT_CHARSET', DEFAULT_CHARSET);

		$this->db->Close();
		$this->layout->flush($charset);
	}
}
