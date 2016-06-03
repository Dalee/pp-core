<?php

use PP\Lib\Engine\AbstractEngine;

class PXEngineJSON extends AbstractEngine {
	protected $result;

	function runModules() {
		$module = $this->modules[$this->area]->getModule();

		$response = PXResponse::getInstance();
		$response->dontCache();

		$this->result = $this->getJson($module);
	}

	function sendJson() {
		$response =& PXResponse::getInstance();
		$response->setContentType('text/javascript');
		$response->send(json_encode($this->result));
		exit;
	}

	function getJson(\PXModule $module) {
		return $module->userJson();
	}

	// Common data encoders for library methods standardization

	public static function toError($value, $koi82utf8 = false) {
		return self::toObject('iserror', $value, $koi82utf8);
	}

	public static function toSuccess($value, $koi82utf8 = false) {
		return self::toObject('isok', $value, $koi82utf8);
	}

	protected static function toObject($key, $value, $encode) {
		return (object)array($key => (object)array('notice' => $value));
	}

}
