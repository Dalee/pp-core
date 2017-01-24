<?php

namespace PP\Lib\Cache\Driver;

use PP\Lib\Cache\CacheInterface;

/**
 * Class Memcached
 * @package PP\Lib\Cache\Driver
 */
class Memcached implements CacheInterface  {
	private $_mcHandler;
	private $expirationTime;
	private $cacheNamespace;

	public  $host = 'localhost';
	public  $port = 11211;

	function __construct($cacheDomain = null, $defaultExpire = 3600, $connectorArgs = null) {
		extension_loaded("memcached") or FatalError(get_class($this) . " error: memcached extension doesn't loaded or installed");
		$this->expirationTime = (int)$defaultExpire;
		$this->cacheNamespace = md5(BASEPATH . $cacheDomain);
		@list($host, $port)   = explode(':', $connectorArgs, 2);
		$this->host           = mb_strlen($host) ? $host : $this->host;
		$this->port           = ($port = (int)$port) > 0 ? $port : $this->port;
		$this->_mcHandler     = $this->connect();
	}

	private function connect() {
		//Avoid persistent connections from cronruns! Non-threadsafe!
		$mcObject = new \Memcached($this->cacheNamespace . getmypid()); //threadsafe cronjob protection

		if (!count($mcObject->getServerList())) {
			//Persistent connection settings must be set only once!
			$mcObject->setOptions(
				array(
					\Memcached::OPT_HASH => \Memcached::HASH_MURMUR,
					\Memcached::OPT_BINARY_PROTOCOL => true,
					\Memcached::OPT_PREFIX_KEY => $this->cacheNamespace,
					\Memcached::OPT_TCP_NODELAY => true //for small data packets
				)
			);

			//TODO: multiple servers support NNADA?!
			$mcObject->addServer($this->host, $this->port) or FatalError(
				get_class($this) . " error: could not add Memcached server at {$this->host}:{$this->port} !"
			);
		}

		return $mcObject;
	}

	function exists($key) {
		$this->_mcHandler->get($this->key($key));
		return $this->_mcHandler->getResultCode() !== \Memcached::RES_NOTFOUND;
	}

	function save($key, $data, $expTime = null){
		$expTime = (int)$expTime;
		$this->_mcHandler->set($this->key($key), $data, $expTime > 0 ? $expTime : $this->expirationTime);
	}

	function load($key) {
		$res = $this->_mcHandler->get($this->key($key)); // look at that: https://github.com/php-memcached-dev/php-memcached/issues/21
		return $this->_mcHandler->getResultCode() == \Memcached::RES_NOTFOUND ? null : $res;
	}

	function delete($key) {
		$this->_mcHandler->delete($this->key($key));
	}

	function deleteGroup($group) {
		$prefix = $this->key($group, true);
		$prefLen = mb_strlen($prefix);
		$allKeys = $this->_mcHandler->getAllKeys();
		if(empty($allKeys)){
			return;
		}
		$toDelete = array();
		foreach($allKeys as $key) {
			if(mb_substr($key, 0, $prefLen) == $prefix){
				$toDelete[] = $key;
			}
		}
		if(empty($toDelete)) {
			return;
		}
		$this->_mcHandler->deleteMulti($toDelete);
	}

	function increment($key, $offset = 1 , $initial = 0, $expTime = null) {
		($expTime > 0) || ($expTime = $this->expirationTime);
		$k = $this->key($key);
		$this->_mcHandler->add($k, $initial, $expTime);
		return $this->_mcHandler->increment($this->key($key), $offset, $initial, $expTime);
	}

	private function key($key, $glob = false) {
		if(is_array($key)) {
			$keyPart   = $this->key(array_shift($key));
			$groupPart = $this->key(array_shift($key));
			return $groupPart . '_' . $keyPart;
		}
		return md5($key) . ($glob ? '_' : '');
	}

	function clear() {
		$this->_mcHandler->flush();
	}
}
