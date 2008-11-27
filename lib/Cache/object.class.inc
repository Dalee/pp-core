<?php
class ObjectCache  implements CacheIF {
	var $cache_dir;
	var $expire     = 3600; // default = 1 hour
	var $orderLevel = 2;

	function ObjectCache($cacheDomain = null) {
		$this->cache_dir = BASEPATH.'/tmp/cache/';

		if ($cacheDomain !== null) {
			$this->cache_dir .= $cacheDomain .'/';
		}
	}

	function setOrderLevel($level=0) {
		$this->orderLevel = $level;
	}

	/** @param integer $expire - seconds */
	function setExpire($expire) {
		$this->expire = $expire;
	}

	function exists($objectId) {
		$fileName = $this->_getFilename($objectId);
		return file_exists($fileName);
	}

	function save($objectId, $data)  {
		$fileName = $this->_getFilename($objectId);
		$serialized = serialize($data);
		$this->_doSave($fileName, $serialized);
	}

	function load($objectId) {
		$unserialized = null;
		$fileName = $this->_getFilename($objectId);	

		if (($serialized = $this->_doLoad($fileName)) !== null) {
			//avoiding error on: unserialize(serialize(false))
			if ($serialized === 'b:0;') {
				$unserialized = false;
			} else {
				if (($tmp = @unserialize($serialized)) !== false) {
					$unserialized = $tmp;
				}
			}
		}

		return $unserialized;
	}

	function clear() {
		$dir = new NLDir($this->cache_dir);
		$dir->emptyfy(false); // false means non remove dirs, only files
	}
	
	/* 
	   private: 
	*/
	function _getFilename($str) {
		$md5 = md5($str);

		if($this->orderLevel) {
			$prefix = '';

			for($i=0; $i < $this->orderLevel; $i++) {
				$prefix .= $md5{0}.'/';
				$md5     = substr($md5, 1);
			}

			MakeDirIfNotExists($this->cache_dir.$prefix);

			$md5 = $prefix.$md5;
		}

		return $this->cache_dir . $md5;
	}

	function _doSave($fileName, $serialized) {
		$fp = @fopen($fileName, 'wb');

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
	function _doLoad($fileName) {
		$serialized = null;
		$fp = @fopen($fileName, 'rb');

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

	function _createCacheDir() {
		MakeDirIfNotExists($this->cache_dir);
	}
}
?>
