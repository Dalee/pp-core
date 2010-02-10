<?php
class PXCacheFile implements IPXCache {
	protected $cache_dir;
	protected $expire;
	protected $orderLevel = 0;

	function PXCacheFile($cacheDomain = null, $defaultExpire = 3600) {
		$this->cache_dir = BASEPATH.'/tmp/cache/';

		if ($cacheDomain !== null) {
			$this->cache_dir .= $cacheDomain .'/';
		}
		
		$this->setExpire((int)$defaultExpire);
		
		$this->_createCacheDir();
	}

	public function getCacheDir() {
		return $this->cache_dir;
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

	function save($objectId, $data, $expTime = null)  {
		$fileName = $this->_getFilename($objectId);
		$serialized = serialize($data);
		$this->_doSave($fileName, $serialized, (int)$expTime);
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
	
	function delete($key){
		$file = $this->_getFilename($key);
		@unlink($file);
	}
	
	function clear() {
		$this->_cleanDir($this->cache_dir.'/');
	}

	/*
	   protected:
	*/

	protected function _cleanDir($dirName) {
		if ($handle = opendir($dirName)) {
			while (false !== ($file = readdir($handle))) {
				if($file == '.' || $file == '..') {
					continue;
				}

				@unlink($dirName.$file);
			}

			closedir($handle);
		}
	}

	protected function _getFilename($str) {
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

	protected function _doSave($fileName, $serialized, $expTime = null) {
		$fp = fopen($fileName, 'wb');

		if ($fp !== false) {
			if (flock($fp, LOCK_EX)) {
				fwrite($fp, (time() + ($expTime ? $expTime : $this->expire)) . $serialized);
				flock($fp, LOCK_UN);
			}

			fclose($fp);
		}
	}

	/** @return String - serialized data */
	protected function _doLoad($fileName) {
		$serialized = null;
		$fp = @fopen($fileName, 'rb');

		if ($fp !== false) {
			if (flock($fp, LOCK_SH)) {
				if (time() < (int)fread($fp, 10)) { // note: valid until 2038 ;)
					while(!feof($fp)) $serialized .= fread($fp, 8192);
				}	
				flock($fp, LOCK_UN);
			}
			fclose($fp);
		}

		return $serialized;
	}

	protected function _createCacheDir() {
		MakeDirIfNotExists($this->cache_dir);
	}
}
?>
