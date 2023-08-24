<?php
define('USERLEVEL_UNAUTHED', 0);
define('USERLEVEL_USER', 1024);
define('USERLEVEL_ADMIN', 8192);
define('USER_SESSION_INTERVAL', time() + 2_592_000);

define('DT_USER', 'suser');
define('DT_GROUP', 'sgroup');
define('LINK_GROUP2USER', 'sgroup2suser');

require_once PPLIBPATH . 'Acl/classes.inc';

use PP\Lib\Auth\NullAuth;

abstract class PXUser
{
	public $id;
	public $login;
	public $passwd;
	public $groups;
	public $data;
	public $auth;

	/** @var PXApplication */
	public $app;

	/**
	 * @var PXDatabase
	 */
	protected $db;

	/** @var PXObjectsACL */
	protected $acl;

	/**
	 * @var PXRequest
	 */
	protected $request;

	public function __construct()
	{
		$this->id = null;
		$this->login = null;
		$this->groups = [0];
		$this->data = [];
		$this->auth = new NullAuth();
	}

	/**
  * @return $this
  */
 public function setDb(PXDatabase $db)
	{
		$this->db = $db;

		$this->db->setUser($this);
		$this->acl = new PXObjectsACL($db, $this);

		return $this;
	}

	/**
  * @return $this
  */
 public function setApp(PXApplication $app)
	{
		$this->app = $app;

		return $this;
	}

	/**
  * @return $this
  */
 public function setRequest(PXRequest $request)
	{
		$this->request = $request;

		return $this;
	}

	public function auth()
	{
		if ($this->isAuthed()) {
			foreach ($this->app->triggers->system as $t) {
				$t->getTrigger()->onAfterUserAuth($this);
			}

			return $this->auth->auth();
		}
		return false;
	}

	public function unAuth()
	{
		return $this->auth->unAuth();
	}

	public function isAdmin()
	{
		foreach ($this->app->modules as $module) {
			if ($module->getName() != 'auth' && $this->can('admin', $module)) {
				return true;
			}
		}

		return false;
	}

	/**
	 * @return bool
	 */
	public function isAuthed()
	{
		return $this->id !== null;
	}

	/**
	 * @return bool
	 */
	public function checkAuth(?array $withCredentials = null): bool
	{
		return false;
	}

	/**
	 * Authorize current user from external user data, without credentials (ex. after confirm email routine)
	 * @param array $userData
	 * @param string|null $authMethod
	 * @return bool
	 */
	public function authorizeAs(array $userData, ?string $authMethod = null): bool
	{
		return false;
	}

	public function getTitle()
	{
		return $this->auth->getTitle();
	}

	/**
	 * @param string|null $req_auth_method
	 * @return array of fully qualified class names
	 */
	public function getAuthMethods($req_auth_method = null)
	{
		$authDriverList = null;

		foreach ($this->app->authrules as $rule => $param) {
			if (!is_null($req_auth_method) && $rule == $req_auth_method && $param['enabled']) { //use required auth method
				$authDriverList = [$rule]; //single element
				break;
			}

			if ($param['enabled']) {
				$authDriverList[] = $rule;
			}
		}

		if (is_null($authDriverList)) {
			$authDriverList[] = NullAuth::class;
		}

		return $authDriverList;
	}

	/**
	 * @param string|null $type
	 * @return string fully qualified class name
	 */
	public function getPrimaryAuthMethod($type = null): string
	{
		$authMethods = $this->getAuthMethods($type);
		return reset($authMethods);
	}
}
