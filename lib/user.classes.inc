<?
define('USERLEVEL_UNAUTHED', 0);
define('USERLEVEL_USER',     1024);
define('USERLEVEL_ADMIN',    8192);

define('USER_SESSION_INTERVAL', time() + 2592000);

define('DT_USER',         'suser');
define('DT_GROUP',        'sgroup');
define('LINK_GROUP2USER', 'sgroup2suser');

require_once 'Auth/interface.class.inc';
require_once('acl.class.inc');

class PXUserInterface {
	var $id;
	var $login;
	var $groups;
	var $level;
	var $data;
	var $auth;

	function PXUserInterface() {
		$this->id     = NULL;
		$this->login  = NULL;
		$this->groups = array(0);
		$this->level  = USERLEVEL_UNAUTHED;
		$this->data   = array();

		$this->auth   = new PXAuthNull();
	}

	function setDb(&$db) {
		$this->db     =& $db;
		$db->setUser($this);

		$this->acl    = new PXObjectsACL($db, $this);
	}

	function setApp(&$app) {
		$this->app =& $app;
	}

	function setRequest(&$request) {
		$this->request =& $request;
	}

	function auth() {
		$this->checkAuth();
		return $this->auth->auth();
	}

	function unAuth() {
		return $this->auth->unAuth();
	}

	function isAdmin() {
		return ($this->level >= USERLEVEL_ADMIN);
	}

	function isAuthed() {
		return !is_null($this->id);
	}

	function checkAuth() {
	}
}

class PXUserNull extends PXUserInterface {
	function can($what, $format, $object) {
		FatalError('Хм... А почему реальный юзер все еще не создан?');
	}
}

class PXUserCron extends PXUserInterface {
	function can($what, $format, $object) {
		return true;
	}
}

class PXUser extends PXUserInterface {
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
