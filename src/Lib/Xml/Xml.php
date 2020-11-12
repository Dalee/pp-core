<?php

namespace PP\Lib\Xml;

class Xml
{

	public const NONE = 0;
	public const ELEMENT = 1;
	public const ATTRIBUTE = 2;
	public const DOC = 9;

	/** @var SimpleXml */
	public $xml;

	/**
	 * Xml constructor.
	 * @param $xmlEntity
	 */
	public function __construct($xmlEntity)
	{

		switch (true) {
			case extension_loaded('simplexml'):
				$this->xml = new SimpleXml($xmlEntity);
				break;

			default:
				$this->xml = (object)['xmlObject' => false];
		}
	}

	/**
	 * @param $fileName
	 * @return bool|SimpleXml
	 */
	public static function load($fileName)
	{
		$instance = new Xml($fileName);
		return $instance->xml->xmlObject ? $instance->xml : false;
	}

	/**
	 * @param $xmlDataInString
	 * @return bool|object
	 */
	public static function loadString($xmlDataInString)
	{
		return Xml::load($xmlDataInString);
	}

	/**
	 * @param $name
	 * @param $value
	 * @return object
	 */
	public static function attributePrototype($name, $value)
	{
		return (object)['name' => $name, 'value' => $value];
	}
}
