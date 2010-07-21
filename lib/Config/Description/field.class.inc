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

	public function __construct($fieldNode, $app, $type) {
		$this->typeDescription = $type;
		$this->app = $app;

		if(!method_exists($fieldNode, 'attributes')) {
			d2($type, false);
			d2($fieldNode);
			d2(pxtrace());
		}

		foreach ($fieldNode->attributes() as $node) {
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
					$this->setParsedStorageType($node, $fieldNode);
					break;

				case 'displaytype':
					$this->setParsedDisplayType($node, $fieldNode);
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

	function setDisplayType($name) {
		$cls = PXDisplayType::getClassByName($name, $this);
		$this->displayType = new $cls($this, $name);
	}

	function setParsedDisplayType($attrNode, $fieldNode) {
		list($cls, $name, $args) = PXDisplayType::parseConfig($attrNode, $fieldNode, $this);

		$this->displayTypeArgs = $args;
		$this->displayType = new $cls($this, $name);
	}

	function setStorageType($name) {
		$cls = PXStorageType::getClassByName($name, $this);
		$this->storageType = new $cls($this, $name);
	}

	function setParsedStorageType($attrNode, $fieldNode) {
		list($cls, $name, $args) = PXStorageType::parseConfig($attrNode, $fieldNode, $this);
		$this->storageTypeArgs = $args;
		$this->storageType = new $cls($this, $name);
	}
}
?>
