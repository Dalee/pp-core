<?php
class PXUserAuthorized extends PXUser {
	/**
	 * @param Mixed $what single rule or array of rules for special behaviour,
	 * which provide "ANY" logic, so we can check multiple rules until first will be allowed
	*/
	function can($what, $format = NULL, $object = NULL) {
		$can = false; //ACL system is restrictive by default
		
		if($object === NULL){
			$object = array('id'=>NULL,'parent'=>NULL);
		}

		foreach((array)$what as $rule){
			if($can = $this->acl->check($rule, $format, $object)){
				break; //check given rules for current user until first return true, "ANY" logic realization
			}
		}

		return $can;
	}

	function checkAuth() {
		foreach (PXRegistry::getApp()->authrules as $rule => $params) {
			if (!$params['enabled']) {
				continue;
			}

			$isAuth = $this->proceedAuth($rule, $params);

			if(!$isAuth) {
				continue;
			}

			$this->setGroups();
			break;
		}

		$this->acl->reload();
		return $this->auth->onAuth();
	}

	private function proceedAuth($rule, $params) {
		$authClass = 'PXAuth'.ucfirst(strtolower($rule));

		if (!class_exists($authClass)) {
			FatalError('Неизвестный способ авторизации ' . $authClass);
		}

		$tmpAuth  = new $authClass(PXRegistry::getApp(), PXRegistry::getRequest(), $this, PXRegistry::getDB(), $params);
		if(!$this->isAuthed()) {
			return false;
		}

		$this->auth = $tmpAuth;
		return true;
	}

	private function setGroups() {
		$this->app = PXRegistry::getApp();
		$this->db  = PXRegistry::getDB();

		if (!isset($this->app->types[DT_GROUP])) {
			return;
		}

		$groups = $this->db->getLinks($this->app->references[LINK_GROUP2USER], DT_USER, $this->id);

		$this->groups = array_unique(array_merge(array(0, (int)$this->data['parent']), array_keys($groups)));
	}

	function aclType() {
		return (count($this->groups) > 1) ? 'bygroup' : 'basic';
	}
}
?>