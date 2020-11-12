<?php

use PP\Lib\Cache\ObjectCache;

/**
 * Генератор пар ключ/значение, использующихся в CAPTCHA
 * Для работы модуля должен существовать каталог, который передаётся конструктору.
 */
class NLBlockingNumbers
{
	public $path;
	public $key;
	public $value;
	public $storage;

	protected static $_storage = null;

	protected static function getStorage()
	{
		if (self::$_storage) {
			return self::$_storage;
		}

		self::$_storage = ObjectCache::get('file', 'captcha');
		return self::$_storage;
	}

	public function __construct()
	{
		$this->storage = self::getStorage();
		$this->path = 'captcha';
		$this->key = -1;
		$this->value = "";
	}

	/**
	 * @return int
	 */
	public function getKey()
	{
		return $this->key;
	}

	public function CreateNew($chars = 4, $alphabet = '0123456789')
	{
		if (!$this->storage) {
			return false;
		}

		$this->key = $lastid = $this->storage->increment($this->key('lastid'));

		$this->value = '';
		$n = mb_strlen($alphabet);
		for ($i = 0; $i < $chars; $i++) {
			$this->value .= $alphabet[mt_rand(0, $n - 1)];
		}

		$this->storage->save($this->key($lastid), $this->value);

		return true;
	}

	public function CheckValueByKey($key, $checkValue, $removeKey = true)
	{
		$key = (int)$key;
		$val = $this->storage->load($this->key($key));

		if ($removeKey) {
			$this->storage->delete($this->key($key));
		}

		return $val === $checkValue;
	}

	public function GetValueByKey($key)
	{
		$key = (int)$key;
		return $this->storage->load($this->key($key));
	}

	public function RemoveKey($key)
	{
		$this->storage->delete($this->key($key));
	}

	protected function key($key)
	{
		return $this->path . '.' . $key;
	}
}
