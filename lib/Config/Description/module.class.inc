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
	var $trigger     = NULL;
	var $package     = NULL;

	const EMPTY_DESCRIPTION = '---';

	public function getModule() {
		$this->load();
		return new $this->class($this->name, $this->settings);
	}

	public function getInstance() {
		throw new DeprecatedException("PXModuleDescription->getInstance() is deprecated. Use PXModuleDescription->getModule()");
	}

	function setAttrs($attrs) {
		foreach($attrs as $item) {
			$name = $item->name;
			$this->$name = utf8_decode($item->value);
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
		$file = strtolower(substr($this->class, 8)).".{$type}.inc";

		if(strlen($this->package) > 0) {
			$package = str_replace(".","/",$this->package);
			$packageLengthWithoutSlashes = strlen(str_replace("/","",$package));
			$file = $package."/".strtolower(substr($file,$packageLengthWithoutSlashes));
		}

		$file = $type.'s/'.$file;

		if (file_exists(BASEPATH.'/local/'.$file)) {
			return BASEPATH.'/local/'.$file;

		} elseif (file_exists(BASEPATH.'/libpp/'.$file)) {
			return BASEPATH.'/libpp/'.$file;
		}
	}

	function lookPackage($dom) {
		$path    = array();
		$package = $dom->xpath("/modules//module[@name='".$this->getName()."']/ancestor::package/@folder");

		if($package) {
			$tmp = $package;
			foreach($tmp  as $pack) {
				$path[] = $pack->nodeValue();
			}

			$this->setToPackage(join(DIRECTORY_SEPARATOR, $path));
		}
	}
}
?>