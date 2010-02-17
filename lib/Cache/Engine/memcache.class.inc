<?php
class PXCacheMemcache implements IPXCache {
	private $_mcHandler;
	private $expirationTime;
	private $cacheNamespace;
	
	const   VALIDATOR     = 4;
	public  $host         = 'localhost';
	public  $port         = 11211;
	private $compressRate = array(20000, 0.2);
	
	function PXCacheMemcache($cacheDomain = null, $defaultExpire = 3600, $connectorArgs = null) {
		extension_loaded("memcache") or FatalError(get_class($this) . " error: memcache extension doesn't loaded or installed");
		$this->expirationTime = (int)$defaultExpire;
		$this->cacheNamespace = md5(BASEPATH . $cacheDomain);
		@list($host, $port)   = explode(':', $connectorArgs, 2);
		$this->host           = strlen($host) ? $host : $this->host;
		$this->port           = ($port = (int)$port) > 0 ? $port : $this->port;
		$this->_mcHandler     = & $this->connect();
	}
	
	private function &connect() {
		$mcObject = new Memcache;
		$mcObject->connect($this->host, $this->port) or FatalError(get_class($this) . " error: failed connect to the Memcached server at {$this->host}:{$this->port} !");
		call_user_func_array(array(&$mcObject, 'setCompressThreshold'), $this->compressRate);
		
		return $mcObject;
	}
	
	private function key($key) {
		if(($validator = $this->_mcHandler->get($this->cacheNamespace)) === false) {
			$this->_mcHandler->set($this->cacheNamespace, $validator = microtime());
		}
		return md5($this->cacheNamespace . $validator . $key);
	}
	
	function exists($key) {
		return $this->load($key) !== false;
	}
	
	function save($key, $data, $expTime = null){
		$this->_mcHandler->set($this->key($key), $data, self::VALIDATOR, ((int)$expTime ? (int)$expTime : $this->expirationTime));
	}
	
	function load($key) {
		$flags = null;
		$res   = $this->_mcHandler->get($this->key($key), $flags);
		return ($flags & self::VALIDATOR) ? $res : null;
	}
	
	function delete($key) {
		$this->_mcHandler->delete($this->key($key));
	}
	
	function clear() {
		$this->_mcHandler->delete($this->cacheNamespace);
	}
}
?>