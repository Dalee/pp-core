<?php

namespace PP\Lib\Engine\Admin;

use Symfony\Component\DependencyInjection\ContainerAwareInterface;
use PXResponse;

/**
 * Class AdminEngineAction.
 *
 * @package PP\Lib\Engine\Admin
 */
class AdminEngineAction extends AbstractAdminEngine {
	var $nextLocation;

	function initModules() {
		$this->area = $this->request->getArea();
		$this->modules = $this->getModule($this->app, $this->area);
	}

	function runModules() {
		// For correct user session expiration handling and admin auth module working
		if (!($this->hasAdminModules() || $this->area == $this->authArea)) {
			return;
		}

		$this->checkArea($this->area);

		$instance = $this->modules[$this->area]->getModule();
		if ($instance instanceof ContainerAwareInterface) {
			$instance->setContainer($this->container);
		}
		$this->nextLocation = $instance->adminAction();
	}

	function redirect() {
		$response = PXResponse::getInstance();
		$response->dontCache();

		switch ($this->request->getAfterActionDeal()) {
			case 'close':
				\CloseAndRefresh();
				break;

			case 'back':
				$nextLocation = $this->nextLocation;
				$nextLocation = !is_null($nextLocation) ? $nextLocation : $this->request->getReferer();

				$response->redirect($nextLocation);
				break;

			default:
				break;
		}
	}
}
