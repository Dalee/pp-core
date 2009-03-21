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
	var $typeDescription;

	public function __construct($nodeattrs, &$app, $type) {
		foreach ($nodeattrs as $node) {
			$name  = $node->name;
			$value = $node->value;

			switch ($name) {
				case 'name':
					$this->name = $value;
					break;

				case 'description':
					$this->description = utf8_decode($value);
					break;

				case 'storagetype':
					$this->setStorageType($value);
					break;

				case 'displaytype':
					$this->setDisplayType($value);
					break;

				case 'handler':
					$this->handler = $value;
					break;

				case 'listed':
					$listed = strtoupper($value);
					$this->listed = $listed == 'TRUE' ? true : false;
					break;

				case 'defaultvalue':
					$this->defaultValue = utf8_decode($value);
					break;

				case 'source':
					$app->directory[$value] = new PXDirectoryDescription($value);
					$this->values = $app->directory[$value];
					$this->source = $value;
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
			$this->setStorageType('string');
		}

		if(is_null($this->displayType)) {
			$this->setDisplayType('TEXT');
		}
	}

	public function setDisplayType($string) {
		$this->displayTypeArgs = explode("|", $string);
		
		$displayType = $this->_getType($this->displayTypeArgs[0], 'PXDisplayType');
		$this->displayType = new $displayType($this, $this->displayTypeArgs[0]);

		array_shift($this->displayTypeArgs);
	}
	
	function setStorageType($fieldType) {
		$storageType = $this->_getType($fieldType, 'PXStorageType');
		$this->storageType = new $storageType($this, $fieldType);
	}
	
	function _getType($type, $baseClass) {
		$inputClass  = $baseClass.ucfirst(strtolower($type));
		return class_exists($inputClass) ? $inputClass : $baseClass;
	}
}
?>