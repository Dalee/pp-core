<?php
class PXAuthSession extends PXAuthSecure {
	protected $session_store              = 'suser_session';
	protected $suser_login_attempts_field = 'suser_login_attempts';
	protected $sid;
	protected $session_lifetime;
	
	protected $_app;
	protected $_user;
	protected $_request;
	protected $_db;
	
	public function __construct($app, $request, $user, $db, $authParams = null) {
		$this->_app     = $app;
		$this->_user    = $user;
		$this->_request = $request;
		$this->_db      = $db;
		
		if (!($this->session_lifetime = (int)ini_get("session.gc_maxlifetime"))) {
			$this->session_lifetime = 1800;
		}
		
		parent::PXAuthSecure($app, $request, $user, $db);
	}

	function encodePasswd($passwd, $toMd5=true) {}

	function auth() {
		setcookie('login',  $this->login, USER_SESSION_INTERVAL, '/', '');
		setcookie('passwd', $this->encodeSession(), 0, '/', '', false, true); //set passwd HttpOnly
	}

	function unAuth() {
		$this->resetPasswd();
		$this->destroySession($this->privateKey($this->sid, $this->_user->data));
	}

	protected function passwd() {
		$this->passwd = $this->_user->passwd = $this->parsePasswd($this->_request->getVar('passwd'));
		$this->sid    = $this->parseSid($this->_request->getCookieVar('passwd'));
	}

	protected function parseSid($sid) {
		if(!is_string($sid)) {
			return;
		}

		return preg_replace('/^[^0-9a-z]{32}$/i', '', substr($sid, 0, 32));
	}

	protected function _proceedAuth($uArray) {
		switch(true) {
			case empty($uArray):
				return;
			case strlen($this->passwd) && $this->validatePassword($this->passwd, $uArray):
				$this->fillUserFields($uArray);
				break;
			case strlen($this->sid) && $this->validateSession($this->sid, $uArray):
				$this->fillUserFields($uArray);
				break;
			case strlen($this->passwd):
				$this->incrementLoginAttempts($uArray);
				break;
		}
	}

	protected function encodeSession() {
		$public_key  = md5(join("--", array(mt_rand(), time(), $this->login, $this->passwdToDB($this->passwd))));
		$private_key = $this->privateKey($public_key, $this->_user->data);

		// save session into database
		$this->_db->modifyingQuery(sprintf("INSERT INTO %s (suser_id, sid, ip) VALUES('%d', '%s', '%s')", $this->session_store, $this->_user->id, $private_key, $this->_request->GetRemoteAddr()), null, null, false);
		
		return $public_key;
	}

	protected function validatePassword($passwd, $uArray) {
		// check login attempts before password validation to prevent enumerate passwords via bruteforce
		// reset login attempts only when valid password entered
		return $this->checkLoginAttempts($uArray) && $this->passwdToDB($this->passwd) == $uArray['passwd'] && $this->resetLoginAttempts($uArray);
	}

	protected function resetPasswd() {
		setcookie('passwd', "", 0, '/', '');
	}

	protected function validateSession($session, $uArray) {
		// run GC to flush altered sessions
		if(mt_rand(0, ini_get("session.gc_divisor")) <= ini_get("session.gc_probability")) {
			$this->runSessionGC();
		}
		
		$private_key = $this->privateKey($session, $uArray);
		
		if(!($session = reset($this->_db->query(sprintf("SELECT * FROM %s WHERE mtime >= NOW() - INTERVAL '%d seconds' AND sid = '%s' LIMIT 1", $this->session_store, $this->session_lifetime, $private_key), true)))) {
			$this->resetPasswd(); // reset staled session cookie
			return false;
		}
		
		// update session last access time
		return ($session['suser_id'] == $uArray['id']) && (bool)$this->_db->modifyingQuery(sprintf("UPDATE %s SET mtime = NOW() WHERE sid = '%s'", $this->session_store, $private_key), null, null, false, true);
	}

	protected function destroySession($private_key) {
		$this->_db->modifyingQuery(sprintf("DELETE FROM %s WHERE sid = '%s'", $this->session_store, $private_key), null, null, false);
	}

	protected function runSessionGC() {
		$this->_db->modifyingQuery(sprintf("DELETE FROM %s WHERE mtime < NOW() - INTERVAL '%d seconds'", $this->session_store, $this->session_lifetime), null, null, false);
	}

	protected function checkLoginAttempts($uArray) {
		/* TODO: check for login attempts here, like this: 
			if(!empty($uArray[$this->suser_login_attempts_field]) && $uArray[$this->suser_login_attempts_field] > N) { 
				do something, ex. check captcha code
				hint: maybe using here external validators for customizing local projects ?
			}
		*/
		return true;
	}

	protected function resetLoginAttempts($uArray) {
		if (!empty($uArray[$this->suser_login_attempts_field])) {
			// reset with cache flush
			$this->_db->modifyingQuery(sprintf("UPDATE %s SET suser_login_attempts = 0 WHERE id = '%d'", DT_USER, $uArray['id']));
		}
		return true;
	}

	protected function incrementLoginAttempts($uArray) {
		if (isset($uArray[$this->suser_login_attempts_field])) {
			// increment with cache flush
			$this->_db->modifyingQuery(sprintf("UPDATE %s SET suser_login_attempts = suser_login_attempts + 1 WHERE id = '%d'", DT_USER, $uArray['id']));
		}
	}

	protected function privateKey($public_key, $user) {
		return md5($this->_request->GetUserAgent() . $this->_request->GetRemoteAddr() . $user['id'] . $user['passwd'] . $public_key);
	}
}
?>