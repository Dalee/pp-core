<?
define('USERLEVEL_UNAUTHED', 0);
define('USERLEVEL_USER',     1024);
define('USERLEVEL_ADMIN',    8192);

define('USER_SESSION_INTERVAL', time() + 2592000);

define('DT_USER',         'suser');
define('DT_GROUP',        'sgroup');
define('LINK_GROUP2USER', 'sgroup2suser');

require_once 'Auth/classes.inc';
require_once('acl.class.inc');

abstract class PXUser {
	var $id;
	var $login;
	var $groups;
	var $level;
	var $data;
	var $auth;

	function __construct() {
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
?>
