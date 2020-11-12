<?php

namespace PP\Lib\Cache\Driver;

use PP\Lib\Cache\CacheInterface;
use PP\Serializer\SerializerAwareInterface;
use PP\Serializer\SerializerAwareTrait;
use PP\Serializer\DefaultSerializer;

/**
 * Class File
 * @package PP\Lib\Cache\Driver
 */
class File implements CacheInterface, SerializerAwareInterface
{
	use SerializerAwareTrait;

	protected $cache_dir;
	protected $expire;
	protected $orderLevel = 0;

	public function __construct($cacheDomain = null, $defaultExpire = 3600)
	{
		$this->serializer = new DefaultSerializer();
		$this->cache_dir = CACHE_PATH . '/';

		if ($cacheDomain !== null) {
			$this->cache_dir .= $cacheDomain . '/';
		}

		$this->setExpire((int)$defaultExpire);

		$this->_createCacheDir();
	}

	public function getCacheDir()
	{
		return $this->cache_dir;
	}

	public function setOrderLevel($level = 0)
	{
		$this->orderLevel = $level;
	}

	/** @param int $expire - seconds */
	public function setExpire($expire)
	{
		$this->expire = $expire;
	}

	public function exists($objectId)
	{
		$fileName = $this->_getFilename($objectId);
		return file_exists($fileName);
	}

	public function save($objectId, $data, $expTime = null)
	{
		$fileName = $this->_getFilename($objectId);
		$serialized = $this->serializer->serialize($data);
		$this->_doSave($fileName, $serialized, (int)$expTime);
	}

	public function load($objectId)
	{
		$fileName = $this->_getFilename($objectId);
		return $this->_doLoad($fileName);
	}

	public function increment($numberKey, $offset = 1, $initial = 0, $expTime = null)
	{
		$fileName = $this->_getFilename($numberKey);
		$fp = @fopen($fileName, 'a+b');

		if ($fp !== false) {
			if (flock($fp, LOCK_EX)) {
				if (!$this->_isExpired($fp)) { // note: valid until 2038 ;)
					$data = '';
					while (!feof($fp)) {
						$data .= fread($fp, 8192);
					}
				}
				isset($data) || $data = $initial;
				$data += $offset;
				ftruncate($fp, 0);
				fseek($fp, 0, SEEK_SET);
				fwrite($fp, (time() + ($expTime ? $expTime : $this->expire)) . $data);
				flock($fp, LOCK_UN);
			}

			fclose($fp);
		}
		if (!isset($data)) {
			FatalError('Reading/writing file error "' . $fileName . '"');
		}
		return isset($data) ? $data : $initial;
	}

	public function loadStaled($objectId)
	{
		$fileName = $this->_getFilename($objectId);
		return $this->_doLoad($fileName, 'expired');
	}

	public function delete($key)
	{
		$file = $this->_getFilename($key);
		@unlink($file);
	}

	public function expired($objectId)
	{
		$fileName = $this->_getFilename($objectId);
		$fp = @fopen($fileName, 'rb');

		$expired = true;

		if ($fp !== false) {
			if (flock($fp, LOCK_SH)) {
				$expired = $this->_isExpired($fp);
				flock($fp, LOCK_UN);
			}

			fclose($fp);
		}

		return $expired;
	}

	public function clear()
	{
		$this->_cleanDir($this->cache_dir . '/');
	}

	public function deleteGroup($gorup)
	{
		$fileGlob = $this->_getFilename($gorup, true);
		$files = glob($fileGlob, GLOB_MARK | GLOB_NOSORT);

		foreach ($files as $file) {
			if (pathinfo($file, PATHINFO_BASENAME) == '.' || pathinfo($file, PATHINFO_BASENAME) == '..') {
				continue;
			}
			if (is_dir($file)) {
				$this->_cleanDir($file . '/');
			} else {
				@unlink($file);
			}
		}
	}

	protected function _cleanDir($dirName)
	{
		if ($handle = opendir($dirName)) {
			while (false !== ($file = readdir($handle))) {
				if ($file == '.' || $file == '..') {
					continue;
				}
				$file = $dirName . $file;
				if (is_dir($file)) {
					$this->_cleanDir($file . '/');
				} else {
					@unlink($file);
				}
			}

			closedir($handle);
		}
	}

	protected function _getFilename($str, $glob = false)
	{
		if (is_array($str)) {
			$key = array_shift($str);
			$group = array_shift($str);
			$md5 = md5($group) . md5($key);
		} else {
			$md5 = md5($str) . ($glob ? '*' : '');
		}

		if ($this->orderLevel) {
			$prefix = explode('', $md5, $this->orderLevel + 1);
			$md5 = array_pop($prefix);
			$prefix = join('/', $prefix) . '/';

			MakeDirIfNotExists($this->cache_dir . $prefix);
			$md5 = $prefix . $md5;
		}

		return $this->cache_dir . $md5;
	}

	protected function _doSave($fileName, $serialized, $expTime = null)
	{
		$fp = fopen($fileName, 'wb');

		if ($fp !== false) {
			if (flock($fp, LOCK_EX)) {
				fwrite($fp, (time() + ($expTime ? $expTime : $this->expire)) . $serialized);
				flock($fp, LOCK_UN);
			}

			fclose($fp);
		}
	}

	/**
	 * @return string - serialized data
	 */
	protected function _doLoad($fileName, $expired = false)
	{
		$unserialized = $serialized = null;
		$fp = @fopen($fileName, 'rb');

		if ($fp !== false) {
			if (flock($fp, LOCK_SH)) {
				if (!$this->_isExpired($fp) || $expired) { // note: valid until 2038 ;)
					while (!feof($fp)) {
						$serialized .= fread($fp, 8192);
					}
				}
				flock($fp, LOCK_UN);
			}
			fclose($fp);
		}

		// avoiding error on: unserialize(serialize(false))
		if ($serialized === 'b:0;') {
			$unserialized = false;
		} elseif ($serialized !== null) {
			$tmp = $this->serializer->unserialize($serialized);
			if ($tmp !== false) {
				$unserialized = $tmp;
			}
		}

		return $unserialized;
	}

	protected function _createCacheDir()
	{
		MakeDirIfNotExists($this->cache_dir);
	}

	protected function _isExpired($fp)
	{
		return time() >= (int)fread($fp, 10); // note: valid until 2038 ;)
	}
}
