<?php
/**
 * Project:     Dalee PHP-classes library
 * File:        dir.class.inc
 *
 * @copyright   Dalee, 2003-2010
 * @author      Michael Beloshapka <mb@dalee.ru>
 * @package     ProximaPortal
 * @subpackage  Filesys
 */

/**
 * Directory manipulation class.
 * @package ProximaPortal
 * @subpackage Filesys
 */
class NLDir
{
	/**
	 * The name of the directory
	 *
	 * @var string
	 */
	public $name;

	/**
	 * The flag of error, that might happened during handling
	 *
	 * @var    bool
	 * @access private
	 */
	public $_block;

	/**
	 * Internal PHP directory object
	 *
	 * @var    object
	 * @access private
	 */
	public $_dirObject;

	/**
	 * The class constructor.
	 *
	 * @param string $catalog name of the directory - absolute (begins with '/' or not)
	 */
	public function __construct($catalog)
	{
		$this->_block = false;
		$catalog = $this->_prepare($catalog);

		if (DIRECTORY_SEPARATOR === '\\') {
			if ($catalog[1] == ':' && $catalog[2] == '\\') {
				$this->name = $catalog;

			} elseif ($catalog[0] . $catalog[1] === '\\\\') {
				$this->name = $catalog;

			} else {
				$this->name = getcwd() . '/' . $catalog;
			}

		} else {
			if ($catalog[0] !== '/') {
				$this->name = getcwd() . '/' . $catalog . '/';
			} else {
				$this->name = $catalog . '/';
			}
		}

		$this->name = $this->_prepare($this->name);

		if (file_exists($catalog) && !is_dir($catalog)) {
			$this->_block = true;
		}
	}

	public function _prepare($catalog)
	{
		$ds = DIRECTORY_SEPARATOR;

		$catalog = preg_replace('|^\\\\|' . REGEX_MOD, '||', (string) $catalog);

		$catalog = str_replace('\\', '/', $catalog);
		$catalog = preg_replace('|\/{2,}|' . REGEX_MOD, '/', $catalog);

		$catalog = str_replace('/', $ds, $catalog);
		$catalog = str_replace('||', '\\\\', $catalog);

		return $catalog;
	}

	/**
	 * Create directory (even parent directory is not exists)
	 *
	 * @param string $permissions permissions for creation
	 * @return int
	 */
	public function Create($permissions = NULL)
	{
		if ($this->_block) {
			return -1;
		}

		$pathElements = explode(DIRECTORY_SEPARATOR, $this->name);
		$path = mb_substr($this->name, 0, 1);

		foreach ($pathElements as $element) {
			$path .= $element . DIRECTORY_SEPARATOR;

			if (!file_exists($path)) {
				if ($permissions != NULL) {
					if (!@mkdir($path, $permissions)) {
						FatalError('Can\'t make dir ' . $path);
					}

				} else {
					if (!@mkdir($path)) {
						FatalError('Can\'t make dir ' . $path);
					}
				}
			} elseif (!is_dir($path)) {
				$this->_block = true;
				return -1;
			}
		}
		return 0;
	}

	/**
	 * Read next file in directory (returns only file name)
	 *
	 * @return string
	 */
	public function Read()
	{
		if ($this->_block) {
			return -1;
		}

		if (!$this->Exist()) {
			return -1;
		}

		if (!is_object($this->_dirObject)) {
			$this->_dirObject = dir($this->name);
		}

		while (true) {
			$entry = $this->_dirObject->read();

			if (($entry[0] ?? '') != '.') {
				break;
			}
		}

		return $entry;
	}

	/**
	 * Read next file in directory (returns full file name with directory name)
	 *
	 * @return string
	 */
	public function ReadFull()
	{
		$entry = $this->Read();

		if ($entry === -1) {
			return FALSE;
		}

		return ($entry !== false) ? $this->name . DIRECTORY_SEPARATOR . $entry : NULL;
	}

	/**
	 * Checking the existence of the directory
	 *
	 * @return bool
	 */
	public function Exist()
	{
		if ($this->_block) {
			return -1;
		}

		return file_exists($this->name);
	}

	/**
	 * Deleting the directory
	 *
	 * @return void
	 */
	public function delete($deleteSelf = true)
	{
		if ($this->_block) {
			return -1;
		}

		if ($this->Exist()) {
			$this->Emptyfy($deleteSelf);
			$this->getFiles(); // делается из за странного бага на windows - каталог не удалялся, без перечитывания списка его файлов

			if ($deleteSelf) {
				rmdir($this->name);
			}
		}
	}

	/**
	 * Deleting all files and subdirectorys from the directory
	 *
	 * @return void
	 */
	public function Emptyfy($deleteSelf = true)
	{
		if ($this->_block) {
			return -1;
		}

		if (is_object($this->_dirObject)) {
			$this->_dirObject = NULL;
		}

		while ($entry = $this->readFull()) {
			if (file_exists($entry)) {
				if (is_dir($entry)) {
					$d = new NLDir($entry);
					$d->delete($deleteSelf);
				} else {
					$this->unlink($entry);
				}
			}
		}
	}

	public function unlink($entry)
	{
		$php_errormsg = null;
  ini_set('track_errors', 1);
		$oldError = error_reporting(0);

		$res = @unlink($entry);

		if (!$res) {
			switch ($php_errormsg) {
				case 'No such file or directory':
					$res = true;
					break;

				default:
					trigger_error($php_errormsg, E_USER_ERROR);
			}
		}

		error_reporting($oldError);

		return $res;
	}

	public function getFiles($type = 'files', $short = false)
	{
		$files = [];
		if ($dir = opendir($this->name)) {
			while (false !== ($file = readdir($dir))) {
				if ($file == '.' || $file == '..') {
					continue;
				}

				switch ($type) {
					case 'files':
					case 'filenames':
						if (!is_file($this->name . $file)) {
							continue(2);
						}
						break;

					case 'dirs':
						if (!is_dir($this->name . $file)) {
							continue(2);
						}
						break;
				}

				$files[] = $short !== false ? $file : $this->name . $file;
			}

			closedir($dir);
			natcasesort($files);
		}
		return $files;
	}

	public function getFileNames($short = true)
	{
		return $this->getFiles('filenames', $short);
	}

	public function getDirs($short = false)
	{
		return $this->getFiles('dirs', $short);
	}

}
