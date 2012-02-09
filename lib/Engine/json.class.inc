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
		$encodedResult = self::jsonEncodeCyr($this->result);
		$response =& PXResponse::getInstance();
		$response->setContentType('text/javascript');
		$response->send(json_encode($encodedResult));
		exit;
	}
	
	function getJson($module) {
		return $module->userJson();
	}
	
	public static function jsonEncodeCyr($object) {
		return convertCyrillicObjectToUtf($object);
	}
	
	// Common data encoders for library methods standardization
	
	public static function toError($value, $koi82utf8 = false){
		return self::toObject('iserror', $value, $koi82utf8);
	}
	
	public static function toSuccess($value, $koi82utf8 = false){
		return self::toObject('isok', $value, $koi82utf8);
	}
	
	protected static function toObject($key, $value, $encode){
		return (object)array($key => (object)array('notice' => $encode ? self::jsonEncodeCyr($value) : $value));
	}
}
?>