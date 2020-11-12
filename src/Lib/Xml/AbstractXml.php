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
	 * @return bool|SimpleXml
	 */
	public function identEntity($testingObject, $nodeType, $fileLoader, $stringLoader)
	{
		set_error_handler(['PP\Lib\Xml\XmlErrors', 'addError'], E_ALL);

		switch (true) {
			case is_a($testingObject, $nodeType):
				$xmlObject = $testingObject;
				break;

			case is_callable($fileLoader) && (file_exists($testingObject) || mb_strlen(getFromArray(parse_url($testingObject), 'scheme'))):
				$xmlObject = @$fileLoader($testingObject);
				break;

			case is_callable($stringLoader) && is_string($testingObject):
				$xmlObject = @$stringLoader($testingObject);
				break;

			default:
				$xmlObject = false;
				break;
		}

		restore_error_handler();

		return $xmlObject;
	}

	/**
	 * {@inheritdoc}
	 */
	abstract public function xpath($query);
}
