<?php

use PP\Lib\Auth\AuthInterface;
use PP\Lib\Auth\AuthException;

class PXUserAuthorized extends PXUser
{
	protected $authDriverRemap = [
		'session' => PP\Lib\Auth\Session::class,
	];

	/**
	 * @param $what string|string[] single rule or array of rules for special behaviour,
	 * which provide "ANY" logic, so we can check multiple rules until first will be allowed
	 * @param $format array
	 * @param $object array
	 * @return bool
	 */
	public function can($what, $format = NULL, $object = NULL)
	{
		$can = false; //ACL system is restrictive by default

		if ($object === NULL) {
			$object = ['id' => NULL, 'parent' => NULL];
		}

		foreach ((array)$what as $rule) {
			if ($can = $this->acl->check($rule, $format, $object)) {
				break; //check given rules for current user until first return true, "ANY" logic realization
			}
		}

		return $can;
	}

	/**
	 * @inheritdoc
	 */
	public function checkAuth(?array $withCredentials = null): bool
	{
		foreach ($this->app->authrules as $rule => $params) {
			if (!$params['enabled']) {
				continue;
			}

			$isAuth = $this->proceedAuth($rule, $params, $withCredentials);
			if (!$isAuth) {
				continue;
			}

			$this->setGroups();
			break;
		}

		$this->acl->reload();
		return $this->auth->onAuth();
	}

	public function authorizeAs(array $userData, ?string $authMethod = null): bool
	{
		if (empty($userData['id'])) {
			return false;
		}

		$authDriverClass = $this->getPrimaryAuthMethod($authMethod);
		$authDriver = $this->makeAuthDriver($authDriverClass, []); //FIXME: auth $params unused for now

		$authDriver->fillUserFields($userData);

		$this->auth = $authDriver;

		if ($this->auth()) {
			$this->setGroups();
			$this->acl->reload();
			return $this->auth->onAuth();
		}

		return false;
	}

	private function makeAuthDriver(string $authDriverClass, array $params = []): AuthInterface
	{
		$authDriverClass = $this->authDriverRemap[$authDriverClass] ?? $authDriverClass;

		if (!class_exists($authDriverClass)) {
			throw new AuthException('Unknown auth driver: ' . $authDriverClass);
		}

		/** @var AuthInterface $authDriver */
		$authDriver = new $authDriverClass($params);
		return $authDriver
			->setApp($this->app)
			->setDb($this->db)
			->setRequest($this->request)
			->setUser($this)
			->setSession(PXRegistry::getSession());
	}

	private function proceedAuth(string $authDriverClass, array $params, ?array $withCredentials): bool
	{
		$authDriver = $this->makeAuthDriver($authDriverClass, $params);

		if (!($withCredentials ? $authDriver->isCredentialsValid($withCredentials) : $authDriver->isAuthorized())) {
			return false;
		}

		$this->auth = $authDriver;
		return true;
	}

	private function setGroups()
	{
		if (!isset($this->app->types[DT_GROUP])) {
			return;
		}

		$groups = $this->db->GetLinks(
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

	public function aclType()
	{
		return (count($this->groups) > 1) ? 'bygroup' : 'basic';
	}
}

