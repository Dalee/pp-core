<?php

/**
* Класс - хранилище.
* Стоится по etc/datatype.xml /model/datatypes/datatype/attribute
*
* @see PXTypeDescription
*
* @subpackage Application
*/
class PXFieldDescription {
	var $name            = NULL;
	var $description     = NULL;
	var $storageType     = NULL;
	var $handler         = NULL;
	var $displayType     = NULL;
	var $displayTypeArgs = NULL;
	var $values          = NULL;
	var $source          = NULL;
	var $defaultValue    = NULL;
	var $listed          = NULL;
	var $noindex         = NULL;

	public function __construct($nodeattrs, &$app) {
		foreach ($nodeattrs as $name => $value) {
			switch ($name) {
				case 'name':
					$this->name = (string)$value;
					break;

				case 'description':
					$this->description = utf8_decode((string)$value);
					break;

				case 'storagetype':
					$this->storageType = (string)$value;
					break;

				case 'displaytype':
					$this->setDisplayType((string)$value);
					break;

				case 'handler':
					$this->handler = (string)$value;
					break;

				case 'listed':
					$listed = strtoupper((string)$value);
					$this->listed = $listed == 'TRUE' ? true : false;
					break;

				case 'defaultvalue':
					$this->defaultValue = utf8_decode((string)$value);
					break;

				case 'source':
					$app->directory[(string)$value] = new PXDirectoryDescription((string)$value);
					$this->values = $app->directory[(string)$value];
					$this->source = (string)$value;
					break;

				case 'noindex': 
					$noindex = strtoupper($value); 
					$this->noindex = $noindex == 'TRUE' ? true : false;

				default:
					d2("Unknown attr name: " . $name .' line: '. __LINE__);
					break;
			}
		}

		if(is_null($this->storageType)) {
			$this->storageType = 'string';
		}

		if(is_null($this->displayType)) {
			$this->setDisplayType('TEXT');
		}
	}

	public function setDisplayType($string) {
		$this->displayTypeArgs = explode("|", $string);
		$this->displayType = $this->displayTypeArgs[0];
		array_shift($this->displayTypeArgs);
	}
}
?>