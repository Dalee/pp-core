<?php

/**
 * Класс - хранилище.
 * Стоится по etc/datatype.xml /model/datatypes/datatype/attribute
 *
 * @see PXTypeDescription
 *
 * @subpackage Application
 */
class PXFieldDescription
{
	public $name = NULL;
	public $description = NULL;

	/** @var PXStorageType|PXStorageTypeAbstractFile */
	public $storageType = null;

	public $storageTypeArgs = NULL;
	public $displayType = NULL;
	public $displayTypeArgs = NULL;
	public $values = NULL;
	public $source = NULL;
	public $defaultValue = NULL;
	public $listed = NULL;
	public $noindex = NULL;
	public $comments = '';
	public $groupName = NULL;

	public function __construct($fieldNode, $app, public $typeDescription)
	{
		$this->app = $app;

		if (is_object($fieldNode)) {
			foreach ($fieldNode->attributes() as $node) {
				$name = $node->name;
				$value = $node->value;

				switch ($name) {
					case 'name':
						$this->name = $value;
						break;

					case 'description':
						$this->description = pp_simplexml_decode_string($value);
						break;

					case 'storagetype':
						$this->setParsedStorageType($node, $fieldNode);
						break;

					case 'displaytype':
						$this->setParsedDisplayType($node, $fieldNode);
						break;

					case 'listed':
						$listed = mb_strtoupper((string) $value);
						$this->listed = $listed == 'TRUE';
						break;

					case 'defaultvalue':
						$this->defaultValue = pp_simplexml_decode_string($value);
						break;

					case 'source':
						$app->directory[$value] = new PXDirectoryDescription($value);
						$this->values =& $app->directory[$value];
						$this->source = $value;
						break;

					case 'noindex':
						$noindex = mb_strtoupper((string) $value);
						$this->noindex = $noindex == 'TRUE';
						break;

					default:
						throw new Exception('Unknown attr name: ' . $name . ' line: ' . __LINE__);
				}
			}
		}
	}

	public function getGroupName($node)
	{
		$groups = @$node->xpath('parent::group');

		if (sizeof($groups) > 0) {
			$groupName = $groups[0]->attributes();
			$this->groupName = pp_simplexml_decode_string($groupName[0]->value);
		}
	}

	public function getComments($node)
	{
		$commentsNode = @$node->xpath("comment");

		if (sizeof($commentsNode) > 0) {
			foreach ($commentsNode as $comment) {
				$this->comments .= pp_simplexml_decode_string($comment->nodeValue());
			}
		}
	}

	public function getDefaultValueNode($node)
	{
		$defaultValueNode = @$node->xpath("defaultvalue");

		if (sizeof($defaultValueNode) > 0) {
			foreach ($defaultValueNode as $value) {
				$value = $value->nodeValue();
				$value = pp_simplexml_decode_string($value);
				$value = trim((string) $value);

				$this->defaultValue .= preg_replace('/^\t+/m' . REGEX_MOD, '', $value);
			}
		}
	}

	public function setDisplayType($name)
	{
		$cls = PXDisplayType::getClassByName($name, $this);
		$this->displayType = new $cls($this, $name);
	}

	public function setParsedDisplayType($attrNode, $fieldNode)
	{
		[$cls, $name, $args] = PXDisplayType::parseConfig($attrNode, $fieldNode, $this);

		$this->displayTypeArgs = $args;
		$this->displayType = new $cls($this, $name);
	}

	public function setStorageType($name)
	{
		$cls = PXStorageType::getClassByName($name);
		$this->storageType = new $cls($this, $name);
	}

	public function setParsedStorageType($attrNode, $fieldNode)
	{
		[$cls, $name, $args] = PXStorageType::parseConfig($attrNode, $fieldNode);
		$this->storageTypeArgs = $args;
		$this->storageType = new $cls($this, $name);
	}
}
