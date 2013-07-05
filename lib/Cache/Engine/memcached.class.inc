<?php

class PXCacheMemcached implements IPXCache {
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
		$this->host           = strlen($host) ? $host : $this->host;
		$this->port           = ($port = (int)$port) > 0 ? $port : $this->port;
		$this->_mcHandler     = $this->connect();
	}

	private function connect() {
		//Avoid persistent connections from cronruns! Non-threadsafe!
		$mcObject = new Memcached($this->cacheNamespace . getmypid()); //threadsafe cronjob protection

		if (!count($mcObject->getServerList())) {
			//Persistent connection settings must be set only once!
			$mcObject->setOptions(
				array(
					Memcached::OPT_HASH => Memcached::HASH_MURMUR,
					Memcached::OPT_BINARY_PROTOCOL => true,
					Memcached::OPT_PREFIX_KEY => $this->cacheNamespace,
					Memcached::OPT_TCP_NODELAY => true //for small data packets
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
		return $this->_mcHandler->getResultCode() !== Memcached::RES_NOTFOUND;
	}

	function save($key, $data, $expTime = null){
		$expTime = (int)$expTime;
		$this->_mcHandler->set($this->key($key), $data, $expTime > 0 ? $expTime : $this->expirationTime);
	}

	function load($key) {
		$res = $this->_mcHandler->get($this->key($key));
		return $this->_mcHandler->getResultCode() == Memcached::RES_NOTFOUND ? null : $res;
	}

	function delete($key) {
		$this->_mcHandler->delete($this->key($key));
	}

	function increment($key, $offset = 1 , $initial = 0, $expTime = null) {
		$k = $this->key($key);
		$this->exists($key) || $this->save($key, $initial, $expTime);
		$this->_mcHandler->increment($this->key($key), $offset, $initial, (int)$expTime);
		return $this->load($key); // increment returns -1 sometimes
	}

	protected function key($key) {
		return md5($key);
	}

	function clear() {
		$this->_mcHandler->flush();
	}
}
