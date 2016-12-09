<?php

namespace PP\Lib\Xml;

class XmlErrors {
	public $errors;

	private function __construct() {
		$this->errors = array();
	}

	private function __clone() {}

	public static function getErrors() {
		$instance = self::getInstance();
		return $instance->errors;
	}

	public static function addError($errno, $errstr, $errfile, $errline, $errcontext) {
		$instance = self::getInstance();
		$instance->errors[] = $errstr;
	}

	public static function getInstance() {
		static $instance;

		if(!is_object($instance)) {
			$instance = new self();
		}

		return $instance;
	}
}
