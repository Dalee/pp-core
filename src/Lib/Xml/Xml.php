<?php

namespace PP\Lib\Xml;

class Xml {

	const NONE = 0;
	const ELEMENT = 1;
	const ATTRIBUTE = 2;
	const DOC = 9;

	/** @var SimpleXml */
	public $xml;

	/**
	 * Xml constructor.
	 * @param $xmlEntity
	 */
	function __construct($xmlEntity) {

		switch (true) {
			case extension_loaded('simplexml'):
				$this->xml = new SimpleXml($xmlEntity);
				break;

			default:
				$this->xml = (object)array('xmlObject' => false);
		}
	}

	/**
	 * @param $fileName
	 * @return bool|SimpleXml
	 */
	public static function load($fileName) {
		$instance = new Xml($fileName);
		return $instance->xml->xmlObject ? $instance->xml : false;
	}

	/**
	 * @param $xmlDataInString
	 * @return bool|object
	 */
	public static function loadString($xmlDataInString) {
		return Xml::load($xmlDataInString);
	}

	/**
	 * @param $name
	 * @param $value
	 * @return object
	 */
	public static function attributePrototype($name, $value) {
		return (object)array('name' => $name, 'value' => $value);
	}
}
