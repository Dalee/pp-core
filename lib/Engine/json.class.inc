<?php
class PXEngineJSON extends PXEngine {
	var $result;

	function __construct(){
		parent::profilerStart();
		parent::__construct();
	}

	function runModules() {
		$module_name = $this->request->getVar('area');

		$module = $this->app->modules[$module_name]->getModule();

		$response =& PXResponse::getInstance();
		$response->dontCache();

		$this->result = $this->getJson($module);
	}

	function sendJson() {
		$encodedResult = $this->jsonEncodeCyr($this->result);
		$response =& PXResponse::getInstance();
		$response->setContentType('text/javascript');
		$response->send(json_encode($encodedResult));
		exit;
	}
	
	function getJson($module) {
		return $module->userJson();
	}
	
	function jsonEncodeCyr($object) {
		if (is_array($object)) {
			$encoded = array();

			foreach ($object as $k => $v) {
				$encoded[$this->jsonEncodeCyr($k)] = $this->jsonEncodeCyr($v);
			}
			$object = $encoded;

		} elseif (is_object($object)) {
			$vars = get_object_vars($object);

			foreach ($vars as $m => $v) {
				$object->$m = $this->jsonEncodeCyr($v);
			}

		} elseif (is_string($object)) {
			$object = myconv('k', 'u', $object);
		}

		return $object;
	}
}
?>