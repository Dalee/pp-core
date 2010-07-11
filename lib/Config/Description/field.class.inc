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
	var $storageTypeArgs = NULL;
	var $handler         = NULL;
	var $displayType     = NULL;
	var $displayTypeArgs = NULL;
	var $values          = NULL;
	var $source          = NULL;
	var $defaultValue    = NULL;
	var $listed          = NULL;
	var $noindex         = NULL;
	var $typeDescription;
	var $comments        = '';
	var $groupName       = NULL;

	public function __construct($nodeattrs, $app, $type) {
		$this->typeDescription = $type;
		$this->app = $app;

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
					$this->setDisplayType(utf8_decode($value));
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
					$this->values =& $app->directory[$value];
					$this->source =  $value;
					break;

				case 'noindex': 
					$noindex = strtoupper($value); 
					$this->noindex = $noindex == 'TRUE' ? true : false;
					break;

				default:
					d2("Unknown attr name: " . $name . ' line: '. __LINE__);
					break;
			}
		}
	}

	function getGroupName($node) {
		$groups = @$node->xpath('parent::group');

		if(sizeof($groups) > 0) {
			$groupName = $groups[0]->attributes();
			$this->groupName = utf8_decode($groupName[0]->value);
		}
	}

	function getComments($node) {
		$commentsNode = @$node->xpath("comment");

		if(sizeof($commentsNode) > 0) {
			foreach($commentsNode as $comment) {
				$this->comments .= utf8_decode($comment->nodeValue());
			}
		}
	}

	function getDefaultValueNode($node) {
		$defaultValueNode = @$node->xpath("defaultvalue");

		if(sizeof($defaultValueNode) > 0) {
			foreach($defaultValueNode as $value) {
				$value = $value->nodeValue();
				$value = utf8_decode($value);
				$value = trim($value);

				$this->defaultValue .= preg_replace('/^\t+/m', '', $value);
			}
		}
	}

	function setDisplayType($string) {
		$this->displayTypeArgs = explode("|", $string);

		$displayType = $this->_getType($this->displayTypeArgs[0], 'PXDisplayType');
		$this->displayType = new $displayType($this, $this->displayTypeArgs[0]);

		array_shift($this->displayTypeArgs);
	}

	function setStorageType($string) {
		$this->storageTypeArgs = explode("|", $string);

		$storageType = $this->_getType($this->storageTypeArgs[0], 'PXStorageType');
		$this->storageType = new $storageType($this, $this->storageTypeArgs[0]);

		array_shift($this->storageTypeArgs);
	}

	function _getType($type, $baseClass) {
		$inputClass  = $baseClass.ucfirst(strtolower($type));
		$autoload    = ($this->app->engineClass != PX_ENGINE_USER) && $baseClass == "PXDisplayType";

		return class_exists($inputClass, $autoload) ? $inputClass : $baseClass;
	}
}
?>
