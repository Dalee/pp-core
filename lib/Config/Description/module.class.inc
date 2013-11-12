<?php
/**
 * Класс - хранилище информации о модуле.
 * Стоится по etc/modules.xml /modules/module.
 *
 * @subpackage Application
 */
class PXModuleDescription {
	var $name        = NULL;
	var $description = NULL;
	var $class       = NULL;
	var $settings    = array();
	var $package     = NULL;
	var $hidetab     = false;
	var $trigger     = false;

	const EMPTY_DESCRIPTION = '---';

	public function getModule() {
		if (!class_exists($this->class, false)) {
			$this->load();
		}

		return new $this->class($this->name, $this->settings, $this);
	}

	public function getInstance() {
		throw new DeprecatedException("PXModuleDescription->getInstance() is deprecated. Use PXModuleDescription->getModule()");
	}

	function setAttrs($attrs) {
		foreach($attrs as $item) {
			$name = $item->name;
			$this->$name = pp_simplexml_decode_string($item->value);
		}
	}

	function checkClass() {
		if(is_null($this->name)) {
			FatalError('Not defined module name');
		}

		if(is_null($this->class)) {
			$cls = 'PXModule';

			if(!is_null($this->package)) {
				$cls .= str_replace('/', '', $this->package);
			}

			$cls .= preg_replace('/[^0-9a-z_]/i'.REGEX_MOD, '', $this->name);

			$this->class = $cls;
		}
	}

	function setToPackage($package) {
		$this->package = $package;
	}

	function getName() {
		return $this->name;
	}

	/**
	* Подключает файл модуля, проверяет сначала local/modules, затем lib/modules
	*/
	function load() {
		$file = $this->getPathToClass();

		if(is_string($file)) {
			include_once $file;
		}
	}

	function getPathToClass($type = 'module') {
		$file = sprintf(
			"%s.%s.inc",
			str_replace('pxmodule', '', strtolower($this->class)),
			$type
		);

		if (strlen($this->package) > 0) {
			$package = str_replace(".", "/", $this->package);
			$packageLengthWithoutSlashes = strlen(str_replace("/", "", $package));
			$file = sprintf("%s/%s", $package, strtolower(substr($file, $packageLengthWithoutSlashes)));
		}

		$file = sprintf("%ss/%s", $type, $file);

		$findBy = array(
			"local",
			"libpp");

		foreach ($findBy as $folder) {
			$requiredPath = sprintf("%s/%s/%s", BASEPATH, $folder, $file);

			if (file_exists($requiredPath)) {
				return $requiredPath;
			}
		}
	}

	function lookPackage($dom) {
		$path    = array();
		$package = $dom->xpath("/modules//module[@name='".$this->getName()."']/ancestor::package/@folder");

		if ($package) {
			$tmp = $package;
			foreach($tmp  as $pack) {
				$path[] = $pack->nodeValue();
			}

			$this->setToPackage(join(DIRECTORY_SEPARATOR, $path));
		}
	}
}
?>
