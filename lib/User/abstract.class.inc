<?php
define('USERLEVEL_UNAUTHED', 0);
define('USERLEVEL_USER', 1024);
define('USERLEVEL_ADMIN', 8192);
define('USER_SESSION_INTERVAL', time() + 2592000);

define('DT_USER', 'suser');
define('DT_GROUP', 'sgroup');
define('LINK_GROUP2USER', 'sgroup2suser');

require_once PPLIBPATH . 'Acl/classes.inc';

use PP\Lib\Auth\Null;

abstract class PXUser {
	var $id;
	var $login;
	var $groups;
	var $data;
	var $auth;

	/** @var PXApplication */
	var $app;

	/**
	 * @var \PXDatabase
	 */
	protected $db;

	/**
	 * @var \PXRequest
	 */
	protected $request;

	function __construct() {
		$this->id = null;
		$this->login = null;
		$this->groups = [0];
		$this->data = [];
		$this->auth = new Null();
	}

	/**
	 * @param PXDatabase $db
	 * @return $this
	 */
	function setDb(PXDatabase $db) {
		$this->db = $db;

		$this->db->setUser($this);
		$this->acl = new PXObjectsACL($db, $this);

		return $this;
	}

	/**
	 * @param PXApplication $app
	 * @return $this
	 */
	function setApp(PXApplication $app) {
		$this->app = $app;

		return $this;
	}

	/**
	 * @param PXRequest $request
	 * @return $this
	 */
	function setRequest(PXRequest $request) {
		$this->request = $request;

		return $this;
	}


	function auth() {
		if ($this->checkAuth()) {
			foreach ($this->app->triggers->system as $t) {
				$t->getTrigger()->onAfterUserAuth($this);
			}

			return $this->auth->auth();
		}
		return false;
	}

	function unAuth() {
		return $this->auth->unAuth();
	}

	function isAdmin() {
		foreach ($this->app->modules as $module) {
			if ($module->name != 'auth' && $this->can('admin', $module)) {
				return true;
			}
		}

		return false;
	}

	/**
	 * @return bool
	 */
	function isAuthed() {
		return $this->id !== null;
	}

	function checkAuth() {
		return false;
	}

	function getTitle() {
		return $this->auth->getTitle();
	}

	/**
	 * @param null $req_auth_method
	 * @return array of fully qualified class names
	 */
	function getAuthMethods($req_auth_method = null) {
		$authDriverList = null;

		foreach ($this->app->authrules as $rule => $param) {
			if (!is_null($req_auth_method) && $rule == $req_auth_method) { //use required auth method
				$authDriverList[0] = $rule; //single element
				break;
			}

			if ($param['enabled']) {
				$authDriverList[] = $rule;
			}
		}

		if (is_null($authDriverList)) {
			$authDriverList[] = 'PP\Lib\Auth\Null';
		}

		return $authDriverList;
	}

	/**
	 * @param null $type
	 * @return string fully qualified class name
	 */
	function getPrimaryAuthMethod($type = null) {
		$authMethods = $this->getAuthMethods($type);
		return reset($authMethods);
	}
}
