<?php
/**
 * Генератор пар ключ/значение, использующихся в CAPTCHA
 * Для работы модуля должен существовать каталог, который передаётся конструктору.
 */

class NLBlockingNumbers {
	var $path;
	var $key;
	var $value;
	var $isblocking;
	var $storage;

	const STORAGE_PROPERTY = 'BLOCKING_NUMBERS_STORAGE';

	protected static $_storage = null;

	protected static function getStorage() {
		if (self::$_storage ) {
			return self::$_storage ;
		}

		try {
			$app = PXRegistry::getApp();
			$storage  = $app->getProperty(self::STORAGE_PROPERTY, 'file');
		} catch (Exception $e) {
			$path       = pp_fetch_file(pp_base_paths('etc'), 'properties.ini');
			$properties = parse_ini_file($path, true);
			$storage    = isset($properties[self::STORAGE_PROPERTY]) ? $properties[self::STORAGE_PROPERTY] : '';
		}
		$storage || $storage = 'file';

		return self::$_storage  = ObjectCache::get($storage, 'captcha');
	}

	function NLBlockingNumbers($path = null, $storage = null) {
		$this->storage    = isset($storage) ? $storage : self::getStorage();
		$this->path       = empty($path) ? 'captcha' : $path;
		$this->isblocking = true;
		$this->key        = -1;
		$this->value      = "";
	}

	function CreateNew($chars = 4, $alphabet = '0123456789') {
		if (!$this->storage) {
			return false;
		}

		$this->key = $lastid = $this->storage->increment($this->key('lastid'));

		$this->value = '';
		$n = strlen($alphabet);
		for ($i = 0; $i < $chars; $i++) {
			$this->value .= $alphabet[mt_rand(0, $n-1)];
		}

		$this->storage->save($this->key($lastid), $this->value);

		return true;
	}

	function CheckValueByKey($key, $checkValue, $removeKey = true) {
		$key = (int)$key;
		$val = $this->storage->load($this->key($key));

		if ($removeKey) {
			$this->storage->delete($this->key($key));
		}

		return $val === $checkValue;
	}

	function GetValueByKey($key) {
		$key = (int)$key;
		return $this->storage->load($this->key($key));
	}

	function RemoveKey($key) {
		$this->storage->delete($this->key($key));
	}

	protected function key($key){
		return $this->path . '.' . $key;
	}
}
?>
