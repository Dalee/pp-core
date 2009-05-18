<?php
class PXCacheMemcache /* implements IPXCache */ {
	var $_mcHandler;
	var $_validator = 4;
	
	function PXCacheMemcache($cacheDomain = null, $defaultExpire = 3600){
		extension_loaded("memcache") or FatalError(get_class($this) . " error: memcache extension doesn't loaded or installed");
		$this->expirationTime = (int)$defaultExpire;
		$this->cacheNamespace = md5(BASEPATH . $cacheDomain);
		//FIXME ! Server address and port must be optional !
		$host                 = "localhost";
		$port                 = 11211;
		$this->_mcHandler     = & $this->connect(serialize(array($host,$port)));
	}
	
		
	function &connect($connectionString){
		$mcObject          =& new Memcache;
		list($host, $port) = unserialize($connectionString);
		$mcObject->connect($host) or FatalError(get_class($this) . " error: failed connect to the Memcached server!");
		$mcObject->setCompressThreshold(20000, 0.2);
		return $mcObject;
	}
	
	function key($key){
		if(($validator = $this->_mcHandler->get($this->cacheNamespace)) === false) {
			$this->_mcHandler->set($this->cacheNamespace, $validator = microtime());
		}
		return md5($this->cacheNamespace . $validator . $key);
	}
	
	function exists($key){
		return $this->load($key) !== false;
	}
	
	function save($key, $data, $expTime = null){
		$this->_mcHandler->set($this->key($key), $data, $this->_validator, ((int)$expTime ? (int)$expTime : $this->expirationTime));
	}
	
	function load($key){
		$flags = null;
		$res   = $this->_mcHandler->get($this->key($key), $flags);
		return ($flags & $this->_validator) ? $res : null;
	}
	
	function delete($key){
		$this->_mcHandler->delete($this->key($key));
	}
	
	function clear(){
		$this->_mcHandler->delete($this->cacheNamespace);
	}
}
?>