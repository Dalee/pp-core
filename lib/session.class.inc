<?php
class PXSession {
	var $sessionName;
	var $saveDirectory;
	
	var $lifeTime;
	var $cookieLifeTime;
	var $onlyCookies;
	var $gcDivisor;
	var $gcProbability;
	
	function PXSession($sessionName, $dirName = null) {
		$this->sessionName    = $sessionName;
		$this->saveDirectory  = $dirName ? $dirName : BASEPATH.'/site/var/sessions/';
		
		$this->lifeTime       = 60*60*24*7;
		$this->cookieLifeTime = 60*60*24*7;
		
		$this->onlyCookies    = true;
		
		$this->gcDivisor      = 100;
		$this->gcProbability  = 1;
	}
	
	function open() {
		ini_set('session.gc_divisor',       $this->gcDivisor);
		ini_set('session.gc_probability',   $this->gcProbability);
		ini_set('session.gc_maxlifetime',   $this->lifeTime);
		ini_set('session.use_only_cookies', $this->onlyCookies);
		
		session_set_cookie_params($this->cookieLifeTime);
		
		MakeDirIfNotExists($this->saveDirectory);
		session_save_path($this->saveDirectory);
		session_name($this->sessionName);
		session_start();
	}

	function close() {
		if (!isset($_SESSION)) return true;
		if (!count($_SESSION)) {
			return session_destroy();
		}
		session_write_close();
		return true;
	}
	
	function destroy() {
		@$this->open();
		session_destroy();
		unset($_SESSION);
		unset($_COOKIE[$this->sessionName]);
	}
	
	function setSessionLifeTime($lifeTime) {
		$this->lifeTime = $lifeTime;
	}
	
	function setCookieLifeTime($lifeTime) {
		$this->cookieLifeTime;
	}
	
	function setGcDivisor($divisor) {
		$this->gcDivisor = $divisor;
	}
	
	function setGcProbability($probability) {
		$this->gcProbability = $probability;
	}
	
	function setOnlyCookies($flag) {
		$this->onlyCookies = $flag;
	}
	
	function setSessionName($name) {
		$this->sessionName = $name;
	}
	
	function &getSessionVars() {
		return $_SESSION['vars'];
	}
	
	function resetVars() {
		$layout = PXRegistry::getLayout();
		if(isset($_SESSION['vars'])) {
			unset($_SESSION['vars']);
		}
	}
	
	function switchSession($sessionName) {
		$request = PXRegistry::getRequest();
		switch(true){
			case isset($_SESSION) && session_name() != $sessionName :
				$this->close();
			case !isset($_SESSION) :
				$sid = preg_match("#^[0-9A-F]{32}$#i", $request->getCookieVar($sessionName), $m) ? $m[0] : md5(uniqid(mt_rand(), true));
				session_id($sid);
				$this->setSessionName($sessionName);
				
				$this->open();
		}
	}
}
?>