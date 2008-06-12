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

	public function getModule() {
		$this->load();
		return new $this->class($this->name, $this->settings);
	}

	public function getInstance() {
		throw new DeprecatedException(
			"PXModuleDescription->getInstance() is deprecated. Use PXModuleDescription->getModule()");
		//return $this->getModule();
	}


	/**
	 * Подключает файл модуля, проверяет сначала local/modules, затем lib/modules
	 */
	private function load() {
		$file = substr(strtolower($this->class), 8).".module.inc";

		$modules_dirname = 'modules';

		$path_to_check = array('local', 'libpp');

		foreach($path_to_check as $path) {
			$full_path = BASEPATH . DIRECTORY_SEPARATOR . $path . DIRECTORY_SEPARATOR .
			 		$modules_dirname . DIRECTORY_SEPARATOR . $file;
			
			if (file_exists($full_path)) {
				include_once $full_path;
				return;
			}//if
		}//foreach
	}
}//class
?>