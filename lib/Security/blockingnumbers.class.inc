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

	function NLBlockingNumbers($path = null) {
		if (is_null($path)) $path = BASEPATH . '/tmp/blocking_nums/';
		$this->path       = preg_replace('|//+|', '/', rtrim(trim($path), '/'));
		$this->isblocking = true;
		$this->key        = -1;
		$this->value      = "";
	}

	function CreateNew($chars = 4, $alphabet = '0123456789') {
		MakeDirIfNotExists($this->path);
		$old_max_key = 0;

		if (!$f = @fopen($this->path . '.lastid', 'a+')) return false;
		flock($f, LOCK_EX);

		list(,,,,,,,$size) = fstat($f);
		if ($size) $old_max_key = intval(fread($f,$size));
		$this->key = $old_max_key + 1;

		$this->value = '';
		$n = strlen($alphabet);
		for($i = 0; $i < $chars; $i++) {
			$this->value .= $alphabet{mt_rand(0, $n-1)};
		}

		if (!$fk = @fopen($this->path . '/' . $this->key, 'w')) {
			fclose($f);
			return false;
		}
		fwrite($fk, $this->value);
		fclose($fk);

		ftruncate($f, 0);
		fseek($f, 0);
		fwrite($f, $this->key);
		fclose($f);
		return true;
	}

	function CheckValueByKey($key, $checkValue, $removeKey = true) {
		$key = (int)$key;
		$r = @file_get_contents($this->path . '/' . $key);
		if ($removeKey) $this->RemoveKey($key);
		return ($r !== false) and ($r === $checkValue);
	}

	function GetValueByKey($key) {
		$key = (int)$key;
		if(file_exists($this->path . '/' . $key)) {
			return file_get_contents($this->path . '/' . $key);
		} else {
			return NULL;
		}
	}

	function RemoveKey($key) {
		@unlink($this->path . '/' . (int)$key);
	}
}
?>
