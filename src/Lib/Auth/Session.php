<?php

namespace PP\Lib\Auth;


class Session extends Secure {

	/** @var string */
	protected $session_store = 'suser_session';

	/** @var string */
	protected $sid;

	/** @var int */
	protected $session_lifetime;


	public function __construct() {
		parent::__construct();

		if (!($this->session_lifetime = (int)ini_get("session.gc_maxlifetime"))) {
			$this->session_lifetime = 1800;
		}
	}

	function encodePasswd($passwd, $toMd5 = true) {
		return null;
	}

	function auth() {
		setcookie('login', $this->login, USER_SESSION_INTERVAL, '/', '');
		setcookie('passwd', $this->encodeSession(), 0, '/', '', false, true); //set passwd HttpOnly
	}

	function unAuth() {
		$this->resetPasswd();
		$this->destroySession($this->privateKey($this->sid, $this->user->data));
	}

	protected function passwd() {
		$this->passwd = $this->user->passwd = $this->parsePasswd($this->request->getVar('passwd'));
		$this->sid = $this->parseSid($this->request->getCookieVar('passwd'));
	}

	protected function parseSid($sid) {
		if (!is_string($sid)) {
			return null;
		}

		return preg_replace('/^[^0-9a-z]{32}$/i', '', substr($sid, 0, 32));
	}

	protected function _proceedAuth($uArray) {
		switch (true) {
			case empty($uArray):
				return;

			case mb_strlen($this->passwd) && $this->validatePassword($this->passwd, $uArray):
			case mb_strlen($this->sid) && $this->validateSession($this->sid, $uArray):
				$this->fillUserFields($uArray);
				break;
		}
	}

	protected function encodeSession() {
		$public_key = md5(join("--", array(mt_rand(), time(), $this->login, self::passwdToDB($this->passwd))));
		$private_key = $this->privateKey($public_key, $this->user->data);

		// save session into database
		$this->db->modifyingQuery(sprintf("INSERT INTO %s (suser_id, sid, ip) VALUES('%d', '%s', '%s')", $this->session_store, $this->user->id, $private_key, $this->request->GetRemoteAddr()), null, null, false);

		return $public_key;
	}

	protected function validatePassword($passwd, $uArray) {
		return self::passwdToDB($this->passwd) == $uArray['passwd'];
	}

	protected function resetPasswd() {
		setcookie('passwd', "", 0, '/', '');
	}

	protected function validateSession($session, $uArray) {
		// run GC to flush altered sessions
		if (mt_rand(0, ini_get("session.gc_divisor")) <= ini_get("session.gc_probability")) {
			$this->runSessionGC();
		}

		$private_key = $this->privateKey($session, $uArray);

		//FIXME: NOW() must be in driver
		$res = $this->db->query(sprintf("SELECT * FROM %s WHERE mtime >= NOW() - INTERVAL '%d seconds' AND sid = '%s' LIMIT 1", $this->session_store, $this->session_lifetime, $private_key), true);
		if (!($session = reset($res))) {
			$this->resetPasswd(); // reset staled session cookie
			return false;
		}

		// update session last access time
		return ($session['suser_id'] == $uArray['id']) && (bool)$this->db->modifyingQuery(sprintf("UPDATE %s SET mtime = NOW() WHERE sid = '%s'", $this->session_store, $private_key), null, null, false, true);
	}

	protected function destroySession($private_key) {
		$this->db->modifyingQuery(sprintf("DELETE FROM %s WHERE sid = '%s'", $this->session_store, $private_key), null, null, false);
	}

	protected function runSessionGC() {
		$this->db->modifyingQuery(sprintf("DELETE FROM %s WHERE mtime < NOW() - INTERVAL '%d seconds'", $this->session_store, $this->session_lifetime), null, null, false);
	}

	protected function privateKey($public_key, $user) {
		return md5($this->request->GetUserAgent() . $this->request->GetRemoteAddr() . $user['id'] . $user['passwd'] . $public_key);
	}
}
