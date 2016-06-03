<?php

namespace PP\Lib\Auth;

use Symfony\Component\HttpFoundation\Session\Session;

abstract class AuthAbstract implements AuthInterface {

	/** @var \PXRequest */
	protected $request;

	/** @var \PXDatabase */
	protected $db;

	/** @var \PXApplication */
	protected $app;

	/** @var \PXUser */
	protected $user;

	/** @var null|Session */
	protected $session;

	protected $login;
	protected $passwd;

	public function __construct($params = []) {
		// params is not used right now..
	}

	/**
	 * {@inheritdoc}
	 */
	public function setRequest(\PXRequest $request) {
		$this->request = $request;

		return $this;
	}

	/**
	 * {@inheritdoc}
	 */
	public function setDb(\PXDatabase $db) {
		$this->db = $db;

		return $this;
	}

	/**
	 * {@inheritdoc}
	 */
	public function setApp(\PXApplication $app) {
		$this->app = $app;

		return $this;
	}

	/**
	 * {@inheritdoc}
	 */
	public function setUser(\PXUser $user) {
		$this->user = $user;

		return $this;
	}

	/**
	 * @param null|Session $session
	 * @return $this
	 */
	public function setSession(Session $session = null) {
		$this->session = $session;

		return $this;
	}


	/**
	 * {@inheritdoc}
	 */
	abstract public function isCredentialsValid();


	protected function fillUserFields($uArray) {
		$user = \PXRegistry::getUser();

		$user->id = $uArray['id'];
		$user->login = $uArray['title'];
		$user->data = $uArray;
		$this->passwd = $user->passwd = $uArray['passwd'];
	}

	function getTitle() {
		return $this->user->login;
	}

	protected function findUser() {
		if (!mb_strlen($this->login)) {
			return null;
		}

		$tmp = $this->db->GetObjectsByFieldLimited(
			$this->app->types[DT_USER],
			true,
			'title',
			$this->login,
			1,
			0
		);

		return count($tmp) ? current($tmp) : null;
	}

	function auth() {
		return true;
	}

	function unAuth() {
		return true;
	}

	/**
	 * Метод-триггер, вызывается в PXUser::checkAuth() после загрузки правил acl,
	 * позволяет выполнить дополнительные проверки.
	 *
	 * @return bool
	 */
	function onAuth() {
		return true;
	}

	protected function encodePasswd($passwd, $toMd5 = true) {
		return $passwd;
	}

	public static function passwdToDB($passwd) {
		return $passwd;
	}

	/**
	 * @param string $login
	 * @return null|string
	 */
	function parseLogin($login) {
		$result = is_string($login)
			? preg_replace('/[^\w\.\@\-]/' . REGEX_MOD, '', mb_substr($login, 0, 255))
			: null;

		return $result;
	}

	/**
	 * @param string $password
	 * @return null|string
	 */
	function parsePasswd($password) {
		$result = is_string($password)
			? $password
			: null;

		return $result;
	}


	protected function _lazySetAuthField($field) {
		$request = \PXRegistry::getRequest();

		$f = $request->getVar($field);

		$parseMethod = sprintf("parse%s", ucfirst($field));
		$f = call_user_func_array([$this, $parseMethod], [$f]);

		if ((!is_string($f)) || (!mb_strlen($f))) {
			$f = (string)$request->getCookieVar($field);
		}

		\PXRegistry::getUser()->$field = $f;
		return $this->$field = $f;
	}


	protected function login() {
		$this->_lazySetAuthField("login");
	}


	protected function passwd() {
		$this->_lazySetAuthField("passwd");
	}
}
