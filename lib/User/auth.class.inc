<?php

use PP\Lib\Auth\AuthInterface;
use PP\Lib\Auth\AuthException;

class PXUserAuthorized extends PXUser {
	protected $authDriverRemap = [
		'session' => 'PP\Lib\Auth\Session',
		'secure' => 'PP\Lib\Auth\Secure',
	];

	/**
	 * @param $what string single rule or array of rules for special behaviour,
	 * which provide "ANY" logic, so we can check multiple rules until first will be allowed
	 * @param $format array
	 * @param $object array
	 * @return bool
	 */
	function can($what, $format = NULL, $object = NULL) {
		$can = false; //ACL system is restrictive by default

		if ($object === NULL) {
			$object = array('id' => NULL, 'parent' => NULL);
		}

		foreach ((array)$what as $rule) {
			if ($can = $this->acl->check($rule, $format, $object)) {
				break; //check given rules for current user until first return true, "ANY" logic realization
			}
		}

		return $can;
	}

	function checkAuth() {
		foreach ($this->app->authrules as $rule => $params) {
			if (!$params['enabled']) {
				continue;
			}

			$isAuth = $this->proceedAuth($rule, $params);
			if (!$isAuth) {
				continue;
			}

			$this->setGroups();
			break;
		}

		$this->acl->reload();
		return $this->auth->onAuth();
	}

	private function proceedAuth($authDriverClass, $params) {
		$authDriverClass = (isset($this->authDriverRemap[$authDriverClass]))
			? $this->authDriverRemap[$authDriverClass]
			: $authDriverClass;

		if (!class_exists($authDriverClass)) {
			throw new AuthException('Unknown auth driver: ' . $authDriverClass);
		}

		/** @var AuthInterface $authDriver */
		$authDriver = new $authDriverClass($params);
		$authDriver
			->setApp($this->app)
			->setDb($this->db)
			->setRequest($this->request)
			->setUser($this)
			->setSession($this->app->engine->getSession());

		if (!$authDriver->isCredentialsValid()) {
			return false;
		}

		$this->auth = $authDriver;
		return true;
	}


	private function setGroups() {
		if (!isset($this->app->types[DT_GROUP])) {
			return;
		}

		$groups = $this->db->getLinks(
			$this->app->references[LINK_GROUP2USER],
			DT_USER,
			$this->id
		);

		$this->groups = array_unique(
			array_merge(
				[0, (int)$this->data['parent']],
				array_keys($groups)
			)
		);
	}

	function aclType() {
		return (count($this->groups) > 1) ? 'bygroup' : 'basic';
	}
}

