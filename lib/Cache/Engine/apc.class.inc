<?php
class PXCacheApc implements IPXCache {
	private $cacheDomain;
	private $expirationTime;
	
	function PXCacheApc($cacheDomain = null, $defaultExpire = 3600){
		extension_loaded("apc") && ini_get("apc.enabled") or FatalError(get_class($this) . " error: APC extension disabled or doesn't installed");
		$this->expirationTime = (int)$defaultExpire;
		$this->cacheDomain    = BASEPATH . $cacheDomain;
	}
	
	private function key($key){
		return md5($this->cacheDomain.$key);
	}
	
	function exists($key){
		apc_fetch($this->key($key), $success);
		return $success;
	}
	
	function save($key, $data, $expTime = null){
		apc_store($this->key($key), $data, ((int)$expTime ? (int)$expTime : $this->expirationTime)) or $this->clear();
	}
	
	function load($key){
		$res = apc_fetch($this->key($key), $success);
		return $success ? $res : null;
	}
	
	function delete($key){
		return apc_delete($this->key($key));
	}
	
	function clear(){
		return apc_clear_cache("user");
	}
}
?>
