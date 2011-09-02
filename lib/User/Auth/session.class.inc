<?php
class PXAuthSession extends PXAuthSecure {
	protected $session_store = 'suser_session';
	protected $_app;
	protected $_user;
	protected $_request;
	protected $_db;
	protected $sid;
	
	function __construct(&$app, &$request, &$user, &$db) {
		$this->_app     = $app;
		$this->_user    = $user;
		$this->_request = $request;
		$this->_db      = $db;
		
		parent::PXAuthSecure($app, $request, $user, $db);
	}

	protected function _proceedAuth($uArray) {
		if(!(count($uArray) && strlen($this->passwd))) {
			return;
		}

		$this->sid = $this->passwd;

		if ($this->passwdToDB($this->passwd) == $uArray['passwd'] || $this->validateSession($this->sid, $uArray)) {
			$this->fillUserFields($uArray);
		}
	}

	function encodePasswd($passwd) {}

	function encodeSession() {
		$public_key  = md5(sprintf("--%s--%s--%s--%s--", mt_rand(), time(), $this->login, $this->passwdToDB($this->passwd)));
		$private_key = $this->privateKey($public_key, $this->_user->data);

		//save session into database
		$this->_db->modifyingQuery(sprintf("INSERT INTO %s (suser_id, sid, ip) VALUES('%s', '%s', '%s')", $this->session_store, $this->_user->id, $private_key, $this->_request->GetRemoteAddr()), null, null, false);
		
		return $public_key;
	}

	function validateSession($session, $uArray) {
		// run GC to flush altered sessions
		if(mt_rand(0, ini_get("session.gc_divisor")) <= ini_get("session.gc_probability")) {
			$this->_db->modifyingQuery(sprintf("DELETE FROM %s WHERE mtime < NOW() - INTERVAL '%s seconds'", $this->session_store, ini_get("session.gc_maxlifetime")), null, null, false);
		}
		
		$private_key = $this->privateKey($session, $uArray);
		
		if(!($session = reset($this->_db->query(sprintf("SELECT * FROM %s WHERE mtime >= NOW() - INTERVAL '%s seconds' AND sid = '%s' LIMIT 1", $this->session_store, ini_get("session.gc_maxlifetime"), $private_key), true)))) {
			return false;
		}
		
		// update session last access time
		return ($session['suser_id'] == $uArray['id']) && (bool)$this->_db->modifyingQuery(sprintf("UPDATE %s SET mtime = NOW() WHERE sid = '%s'", $this->session_store, $private_key), null, null, false, true);
	}

	function auth() {
		setcookie('login',  $this->login, USER_SESSION_INTERVAL, '/', '');
		setcookie('passwd', $this->encodeSession(), 0, '/', '');
	}

	function unAuth() {
		setcookie('passwd', 0, 0, '/', '');
		$this->_db->modifyingQuery(sprintf("DELETE FROM %s WHERE sid = '%s'", $this->session_store, $this->privateKey($this->sid, $this->_user->data)), null, null, false);
	}

	protected function privateKey($public_key, $user) {
		return md5($this->_request->GetUserAgent() . $this->_request->GetRemoteAddr() . $user['id'] . $user['passwd'] . $public_key);
	}
}
?>