<?php

namespace PP\Lib\Engine\Admin;

use Symfony\Component\DependencyInjection\ContainerAwareInterface;
use PXResponse;

/**
 * Class AdminEngineJson.
 *
 * @package PP\Lib\Engine\Admin
 */
class AdminEngineJson extends AbstractAdminEngine {

	protected $result;

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
		$this->result = $instance->adminJson();
	}

	function sendJson() {
		$body = json_encode($this->result);

		$response = PXResponse::getInstance();
		$response->setContentType('text/javascript');
		$response->setLength(strlen($body));
		$response->send($body);
		exit;
	}
}
