<?php

namespace PP\Properties;

use PP\Lib\Xml\SimpleXmlNode;

class PropertyTypeBuilder
{
	public const TYPE_ID = 'sys_property';

	/**
	 * @throws \Exception
	 */
	public static function create(\PXApplication $app, string $valueDisplayType = 'TEXT')
	{
		$objectDef = [
			'id' => [
				'name' => 'id',
				'description' => 'PK',
				'displaytype' => 'HIDDEN',
				'storagetype' => 'pk',
			],
			OBJ_FIELD_UUID => [
				'name' => 'sys_uuid',
				'description' => '',
				'displaytype' => 'HIDDEN',
				'storagetype' => 'string',
			],
			'name' => [
				'name' => 'name',
				'description' => $app->langTree->getByPath('module_properties.table.name.rus'),
				'displaytype' => 'TEXT',
				'storagetype' => 'string',
			],
			'value' => [
				'name' => 'value',
				'description' => $app->langTree->getByPath('module_properties.table.value.rus'),
				'displaytype' => $valueDisplayType,
				'storagetype' => 'string',
			],
			'description' => [
				'name' => 'description',
				'description' => $app->langTree->getByPath('module_properties.table.description.rus'),
				'displaytype' => 'TEXT|500|100',
				'storagetype' => 'string',
			],
		];

		return self::buildTypeFromArray($app, $objectDef);
	}

	/**
	 * @param \PXApplication $app
	 * @param array $objectDef
	 *
	 * @return \PXTypeDescription
	 * @throws \Exception
	 */
	protected static function buildTypeFromArray(\PXApplication $app, array $objectDef): \PXTypeDescription
	{
		$typeDescription = new \PXTypeDescription();
		$typeDescription->id = self::TYPE_ID;
		$typeDescription->title = $app->langTree->getByPath('module_properties.table.name.rus');

		foreach ($objectDef as $dataDef) {
			$_tmpSource = null;
			if (isset($dataDef['source'], $app->directory[$dataDef['source']])) {
				$_tmpSource = $dataDef['source'];
				unset($dataDef['source']);
			}

			$field = new \PXFieldDescription(
				self::createAttributeNode($dataDef),
				$app,
				$typeDescription
			);

			$field->listed = false;
			if ($_tmpSource !== null) {
				$field->source = $_tmpSource;
				$field->values = &$app->directory[$_tmpSource];
			}

			$typeDescription->addField($field);
			$typeDescription->assignToGroup($field);
		}

		return $typeDescription;
	}

	/**
	 * @param array $fieldData
	 *
	 * @return SimpleXmlNode
	 */
	protected static function createAttributeNode(array $fieldData): SimpleXmlNode
	{
		$attr = new \SimpleXMLElement("<attribute/>");
		foreach ($fieldData as $k => $v) {
			$attr->addAttribute($k, $v);
		}

		return new SimpleXmlNode($attr);
	}
}
