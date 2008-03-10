<?php

class ObjectCache {
	
	var $cache_dir;
	var $expire = 3600; // default = 1 hour
	
	function ObjectCache($cache_domain = null) {

		$this->cache_dir = BASEPATH.'/var/objcache/';

		if ($cache_domain !== null) {
			$this->cache_dir .= $cache_domain .'/';
		}
		$this->_create_cache_dir();
	}
	
	/** @param integer $expire - seconds */
	function setExpire($expire) {
		$this->expire = $expire;
	}

	function exists($object_id) {
		$filename = $this->cache_dir . md5($object_id);		
		return file_exists($filename);
	}

	function save($object_id, $data)  {
		$filename = $this->cache_dir . md5($object_id);
		$serialized = serialize($data);
		$this->_do_save($filename, $serialized);
	}

	function load($object_id) {
		$unserialized = null;
		$filename = $this->cache_dir . md5($object_id);		
		if (($serialized = $this->_do_load($filename)) !== null) {
			//avoiding error on: unserialize(serialize(false))
			if ($serialized === 'b:0;') {
				$unserialized = false;
			} else {
				if (($_tmp = @unserialize($serialized)) !== false) {
					$unserialized = $_tmp;
				}
			}
		}
		return $unserialized;
	}

	function clear() {
		$dir = new NLDir($this->cache_dir);
		$dir->emptyfy();
	}
	
	/* fixme: not implemented
	 *
	 * must delete cache domain dir
	 * 

	function destroy() {
		$dir = new NLDir($this->cache_dir);
		$dir->emptyfy();
	}
	*/

	/* 
	   private: 
	*/

	function _do_save($filename, $serialized) {
		
		$fp = @fopen($filename, 'wb');		
		if ($fp !== false) {
			if (flock($fp, LOCK_EX)) {			
				ftruncate($fp, 0);
				rewind($fp);
				fwrite($fp, $serialized);
				flock($fp, LOCK_UN);
			}
			fclose($fp);
		}
	}

	/** @return String - serialized data */
	function _do_load($filename) {
		$serialized = null;
		$fp = @fopen($filename, 'rb');
		if ($fp !== false) { 
			if (flock($fp, LOCK_SH)) {
				$stat = fstat($fp);
				$now  = mktime();
				//if size > 0 and not expired
				if ($stat['size'] > 0 && $now - $stat['mtime'] < $this->expire) {
					$serialized = fread($fp, $stat['size']);
				}
				flock($fp, LOCK_UN);
			}
			fclose($fp);
		}
		return $serialized;
	}

	function _create_cache_dir() {
		$dir = new NLDir($this->cache_dir);
		return $dir->create();
	}
}
?>
