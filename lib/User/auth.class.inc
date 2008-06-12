<?php
class PXUserAuthorized extends PXUser {
	function can($what, $format, $object) {
		return $this->acl->check($what, $format, $object);
	}

	function checkAuth() {
		$app     =& $this->app;
		$db      =& $this->db;
		$request =& $this->request;
		$user    =& $this;

		foreach ($app->authrules as $rule => $params) {
			if (!$params['enabled']) {
				continue;
			}

			$authClass = 'PXAuth'.ucfirst(strtolower($rule));
			if (!class_exists($authClass)) {
				FatalError('Неизвестный способ авторизации '.$authClass);
			}

			$tmpAuth  = new $authClass($app, $request, $user, $db, $params);
			if (!$this->isAuthed()) {
				continue;
			}

			$this->auth =& $tmpAuth;

			if (isset($app->types[DT_GROUP])) {
				$groups = $db->getLinks($app->references[LINK_GROUP2USER], DT_USER, $this->id);
				$this->groups = array_unique(array_merge(array(0, (int)$this->data['parent']), array_keys($groups)));
			}
			break;
		}

		$this->acl->reload();
	}
}
?>
