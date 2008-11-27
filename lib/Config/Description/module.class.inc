<?
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
	var $access      = NULL;
	var $settings    = array();
	var $hidetab     = NULL;
	var $trigger     = NULL;
	var $package     = NULL;

	public function getModule() {
		$this->load();
		return new $this->class($this->name, $this->settings);
	}

	public function getInstance() {
		throw new DeprecatedException("PXModuleDescription->getInstance() is deprecated. Use PXModuleDescription->getModule()");
	}

	function setAttrs($attrs) {
		foreach($attrs as $item) {
			$name = $item->getName();
			$this->$name = utf8_decode((string)$item);
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

	function getPathToClass() {
		$file = strtolower(substr($this->class, 8)).".module.inc";

		if(strlen($this->package) > 0) {
			$package = str_replace(".","/",$this->package);
			$packageLenghtWithOutSlashes = strlen(str_replace("/","",$package));
			$file = $package."/".strtolower(substr($file,$packageLenghtWithOutSlashes));
		}

		if (file_exists(BASEPATH.'/local/modules/'.$file)) {
			return BASEPATH.'/local/modules/'.$file;
		} elseif (file_exists(BASEPATH.'/libpp/modules/'.$file)) {
			return BASEPATH.'/libpp/modules/'.$file;
		}
	}

	function lookPackage($xml) {
		$package = $xml->xpath("/modules//module[@name='".$this->getName()."']/ancestor::package/@folder");

		if($package) {
			$path = '';

			foreach($package  as $pack) {
				$path .= '/'.(string)$pack;
			}

			$this->setToPackage($path);
		}
	}
}

?>