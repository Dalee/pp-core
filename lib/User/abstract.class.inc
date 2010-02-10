<?php
define('USERLEVEL_UNAUTHED', 0);
define('USERLEVEL_USER',     1024);
define('USERLEVEL_ADMIN',    8192);

define('USER_SESSION_INTERVAL', time() + 2592000);

define('DT_USER',         'suser');
define('DT_GROUP',        'sgroup');
define('LINK_GROUP2USER', 'sgroup2suser');

require_once 'User/Auth/classes.inc';
require_once 'Acl/classes.inc';

abstract class PXUser {
	var $id;
	var $login;
	var $groups;
	var $data;
	var $auth;
	var $app;

	function __construct() {
		$this->id     = NULL;
		$this->login  = NULL;
		$this->groups = array(0);
		$this->data   = array();
		$this->app    = PXRegistry::getApp();

		$this->auth = new PXAuthNull();
	}

	function setAcl() {
		PXRegistry::getDb()->setUser($this);
		$this->acl = new PXObjectsACL(PXRegistry::getDb(), $this);
	}

	// FIXME
	function auth() {
		if(strtolower(get_class($this->auth)) != 'pxauthnull' || $this->checkAuth()){
			return $this->auth->auth();
		}
		return false;
	}

	function unAuth() {
		return $this->auth->unAuth();
	}

	function isAdmin() {
		foreach($this->app->modules as $module) {
			if($module->name != 'auth' && $this->can('admin', $module)) {
				return true;
			}
		}

		return false;
	}

	function isAuthed() {
		return !is_null($this->id);
	}

	function checkAuth() {
	}

	function getAuthMethods($req_auth_method = null) {
		$authmethods = null;

		foreach($this->app->authrules as $rule => $param){
			if(!is_null($req_auth_method) && $rule == $req_auth_method){ //use required auth method
				$authmethods[0] = $rule; //single element
				break;
			}

			if($param['enabled']) {
				$authmethods[] = $rule;
			}
		}

		if(is_null($authmethods)) {
			$authmethods[] = 'null';
		}

		return $authmethods;
	}

	function getPrimaryAuthMethod($type = null){
		return 'PXAuth' . ucfirst(reset($this->getAuthMethods($type)));
	}
}
?>