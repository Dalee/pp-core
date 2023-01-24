<?php

namespace PP\Lib\Xml;

/**
 * Class AbstractXml
 * @package PP\Lib\Xml
 */
abstract class AbstractXml implements XmlInterface
{

	public $xmlObject;
	public $errors;

	/**
  * @param string|object $testingObject
  * @param int $nodeType
  * @param string $fileLoader
  * @param string $stringLoader
  */
 public function identEntity($testingObject, $nodeType, $fileLoader, $stringLoader): bool|\SimpleXMLElement
	{
		set_error_handler(\PP\Lib\Xml\XmlErrors::addError(...), E_ALL);

		$xmlObject = match (true) {
      is_a($testingObject, $nodeType) => $testingObject,
      is_callable($fileLoader) && (file_exists($testingObject) || mb_strlen((string) getFromArray(parse_url($testingObject), 'scheme'))) => @$fileLoader($testingObject),
      is_callable($stringLoader) && is_string($testingObject) => @$stringLoader($testingObject),
      default => false,
  };

		restore_error_handler();

		return $xmlObject;
	}

	/**
	 * {@inheritdoc}
	 */
	abstract public function xpath($query);
}
