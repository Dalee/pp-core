<?php

class PXUserAuthorized extends PXUser {
	function can($what, $format = NULL, $object = NULL) {
		if($format === NULL){
			$format =  new PXTypeDescription;
		}

		if($object === NULL){
			$object = array('id'=>NULL,'parent'=>NULL);
		}

		return $this->acl->check($what, $format, $object);
	}

	function checkAuth() {
		foreach (PXRegistry::getApp()->authrules as $rule => $params) {
			if (!$params['enabled']) { 
				continue;
			}

			if(!$this->proceedAuth($rule, $params)) {
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
		
		$tmpAuth  = new $authClass(PXRegistry::getApp(), PXRegistry::getRequest(), $this,
			PXRegistry::getDB(), $params);

		if(!$this->isAuthed()) 
			return false;

		$this->auth = $tmpAuth;
		return true;
	}

	private function setGroups() {
		$app = PXRegistry::getApp();
		$db  = PXRegistry::getDB();
		
		if (!isset($app->types[DT_GROUP])) return;
		
		$groups = $db->getLinks($app->references[LINK_GROUP2USER], DT_USER, $this->id);
		$this->groups = array_unique(array_merge(array(0, (int)$this->data['parent']), array_keys($groups)));
	}

	function aclType() {
		return (count($this->groups) > 1) ? 'bygroup' : 'basic';
	}
}
?>
