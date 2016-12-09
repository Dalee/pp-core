<?php

use PP\Properties\PropertyLoader;
use Ramsey\Uuid\Uuid;
use PP\Lib\Xml\SimpleXmlNode;

/**
 * Check documentation: libpp/docs/properties.module.md
 *
 * Class PXModuleProperties
 */
class PXModuleProperties extends PXModule {

	/** @var array */
	protected $predefinedPropertyDefList = [];

	/**
	 * {@inheritdoc}
	 */
	public function __construct($area, $settings, $selfDescription) {
		parent::__construct($area, $settings, $selfDescription);

		$this->parsePredefinedProperties((array)$this->settings['attribute']);
	}

	/**
	 * {@inheritdoc}
	 */
	public static function getAclModuleActions() {
		$defaults = parent::getAclModuleActions();
		$defaults['sys_properties_edit'] = PXRegistry::getApp()
			->langTree
			->getByPath('module_acl_rules.actions.sys_properties_edit.rus');

		return $defaults;
	}

	/**
	 * {@inheritdoc}
	 */
	public function adminIndex() {
		// get sid
		$sid = $this->request->getSid();
		$sid = (empty($sid)) ? 'pub' : $sid;
		$sid = ($this->isPowerUser()) ? $sid : 'pub';

		// build menu
		$menu = [];
		$menu['pub'] = $this->app->langTree
			->getByPath('module_properties.menu.general.rus');

		if ($this->isPowerUser()) {
			$menu['sys'] = $this->app->langTree
				->getByPath('module_properties.menu.system.rus');
		}
		$this->layout
			->assignKeyValueList('INNER.0.0', $menu, $sid);

		// build table
		$colsDef = [
			'description' => $this->app->langTree->getByPath('module_properties.table.description.rus'),
			'name' => $this->app->langTree->getByPath('module_properties.table.name.rus'),
			'value' => $this->app->langTree->getByPath('module_properties.table.value.rus'),
		];
		$table = (new PXAdminTableSimple($colsDef))
			->setTableId('properties')
			->showEven()
			->setNullText('Пустое значение')
			->setControls($this->buildAdminPopupUrl('edit', true), 'редактировать этот параметр', 'edit', false, true)
			->setData($this->getPropertyList($sid));
		if ($this->isPowerUser()) {
			$table->setControls(
				$this->buildAdminActionUrl('delete', true),
				'удалить этот параметр',
				'del',
				true,
				false
			);
		}

		$table->addToParent('INNER.1.0');

		// build additional controls
		if ($this->isPowerUser()) {
			$link = sprintf("Popup('%s');", $this->buildAdminPopupUrl('main'));

			$button = (new \PXControlButton('Параметр'))
				->setClickCode($link)
				->setClass('add');

			$button->addToParent('INNER.1.1');
		}
	}

	/**
	 * @param $id
	 * @return array|null
	 */
	protected function getPropertyDefById($id) {
		if (isset($this->predefinedPropertyDefList[$id])) {
			$propertyDef = $this->predefinedPropertyDefList[$id];
			$propertyDef['id'] = $propertyDef['name'];
			$propertyDef['sys_uuid'] = '';
			$propertyDef['value'] = '';

		} else {
			$propertyDef = PropertyLoader::getPropertyById($id, $this->db);
			if ($propertyDef === null) {
				$propertyDef = [
					'id' => null,
					'sys_uuid' => '',
					'name' => '',
					'description' => '',
					'value' => '',
				];
			}
		}

		return $propertyDef;
	}

	/**
	 * {@inheritdoc}
	 */
	public function adminPopup() {
		$id = $this->request->getId();

		$propertyDef = $this->getPropertyDefById($id);
		$typeDef = $this->getTypeDescription($propertyDef);

		$form = new PXAdminForm(
			[
				'id' => $propertyDef['id'],
				'sys_uuid' => $propertyDef['sys_uuid'],
				'name' => $propertyDef['name'],
				'description' => $propertyDef['description'],
				'value' => $propertyDef['value'],
			], $typeDef
		);

		$form->setAction('main');
		$form->setArea($this->area);
		$form->setTitle($this->app->langTree->getByPath('module_properties.action.add.rus'));
		$form->getForm();
	}

	/**
	 * {@inheritdoc}
	 */
	public function adminAction() {
		$id = $this->request->getId();
		$sid = $this->request->getSid();

		$propertyDef = $this->getPropertyDefById($id);
		if (!$this->isPowerUser() && $this->isPropertySystem($propertyDef)) {
			FatalError("Access denied");
		}

		$typeDef = $this->getTypeDescription($propertyDef);
		$object = $this->request->getContentObject($typeDef);

		$redirect = '/admin/?area='.$this->area.'&sid='.$sid;
		switch ($this->request->getAction()) {
			case 'main':
				$redirect = $this->saveAction($id, $object);
				break;

			case 'delete':
				$this->deleteAction($id, $object);
				break;
		}

		return $redirect;
	}

	/**
	 * @param mixed $id
	 * @param array $object
	 */
	protected function deleteAction($id, $object) {
		$id = is_numeric($id)? $id : null;
		if (empty($id)) {
			return;
		}

		$deleteQuery = sprintf("DELETE FROM %s WHERE id=%s", DT_PROPERTIES, $this->db->EscapeString($id));
		$this->db->ModifyingQuery($deleteQuery, DT_PROPERTIES);
	}

	/**
	 * @param mixed $id
	 * @param array $object
	 * @return string
	 */
	protected function saveAction($id, $object) {
		unset($object['id']);
		if (empty($object['sys_uuid'])) {
			$object['sys_uuid'] = Uuid::uuid4()->toString();
		}

		$fields = array_keys($object);
		$values = array_values($object);
		$id = is_numeric($id)? $id : null;

		if (empty($id)) {
			$id = $this->db->InsertObject(DT_PROPERTIES, $fields, $values);
		} else {
			$this->db->UpdateObjectById(DT_PROPERTIES, $id, $fields, $values);
		}

		$redirect = $this->buildAdminPopupUrl('main', true);
		$redirect .= urlencode($id);

		return $redirect;
	}

	/**
	 * @param array $publicProperties
	 */
	protected function parsePredefinedProperties(array $publicProperties) {
		foreach ($publicProperties as $propertyStrDef) {
			$propertyDef = [];
			$formatString = str_replace('|', '&', $propertyStrDef);

			parseStrMagic($formatString, $propertyDef);
			if (!isset($propertyDef['name'])) {
				FatalError("В параметрах модуля, для одного из полей отсутствует обязательный параметр name");
			}

			$propertyDef['description'] = isset($propertyDef['description'])
				? pp_simplexml_encode_string($propertyDef['description'])
				: $propertyDef['name'];

			$propertyDef['storagetype'] = isset($propertyDef['storagetype'])
				? $propertyDef['storagetype']
				: 'string';

			$propertyDef['displaytype'] = isset($propertyDef['displaytype'])
				? str_replace(',', '|', $propertyDef['displaytype'])
				: 'TEXT';

			$key = $propertyDef['name'];
			$this->predefinedPropertyDefList[$key] = $propertyDef;
		}
	}

	/**
	 * Is current user allowed to modify system properties?
	 *
	 * @return bool
	 */
	protected function isPowerUser() {
		return $this->user->can('sys_properties_edit', $this->app->modules[$this->area]);
	}

	/**
	 * @param array $propertyRaw
	 * @return bool
	 */
	protected function isPropertySystem(array $propertyRaw) {
		// public property - property listed in module settings and don't have SYS_/sys_ prefix
		$key = $propertyRaw['name'];
		$prefix = mb_strtolower(mb_substr($key, 0, mb_strlen('sys_')));

		return ($prefix === 'sys_' || !isset($this->predefinedPropertyDefList[$key]));
	}

	/**
	 * Get property list with mixin of predefined properties
	 *
	 * @param string $sid
	 * @return array
	 */
	protected function getPropertyList($sid) {
		$propertyList = PropertyLoader::getRawPropertyList($this->db);

		// filter property list based on sid
		$propertyList = array_filter(
			$propertyList,
			function ($propertyRaw) use (&$sid) {
				if ($sid === 'pub') {
					return $this->isPropertySystem($propertyRaw) === false;
				} else {
					return $this->isPropertySystem($propertyRaw);
				}
			}
		);

		// mixin public properties defined in module definition even if they are not set
		$propertyListDef = array_filter(
			$this->predefinedPropertyDefList,
			function ($propertyDef) use ($sid) {
				if ($sid === 'pub') {
					return $this->isPropertySystem($propertyDef) === false;
				} else {
					return $this->isPropertySystem($propertyDef);
				}
			}
		);

		// extract properties already present in database
		$createdDef = array_filter(
			$propertyList,
			function ($propertyRaw) use (&$propertyListDef) {
				return isset($propertyListDef[$propertyRaw['name']]);
			}
		);

		$createdDef = array_flat($createdDef, 'name');
		$missingDef = array_diff_key($propertyListDef, $createdDef);

		// mixin available properties
		foreach ($missingDef as $propertyDef) {
			$format = $this->getTypeDescription($propertyDef);
			$table = [
				[
					'id' => $propertyDef['name'], // hack to
					'sys_created' => null,
					'sys_modified' => null,
					'name' => $propertyDef['name'],
					'description' => $propertyDef['description'],
					'value' => null,
				],
			];

			$this->db->_NormalizeTable($table, $format, true);
			$propertyList = array_merge($table, $propertyList);
		}

		// transform and mixin created properties..
		$propertyList = array_map(
			function ($propertyRaw) use (&$createdDef) {
				$key = $propertyRaw['name'];
				if (isset($createdDef[$key])) {
					$format = $this->getTypeDescription($createdDef[$key]);
					$table = [
						[
							'id' => $propertyRaw['id'],
							'sys_created' => null,
							'sys_modified' => null,
							'name' => $propertyRaw['name'],
							'description' => $propertyRaw['description'],
							'value' => $propertyRaw['value'],
						],
					];

					$this->db->_NormalizeTable($table, $format, true);
					return $table[0];
				}
				return $propertyRaw;
			},
			$propertyList
		);

		// resort all properties
		usort(
			$propertyList,
			function ($left, $right) {
				return mb_strcasecmp($left['name'], $right['name']);
			}
		);

		return $propertyList;
	}

	/**
	 * @param array $object
	 * @return PXTypeDescription
	 */
	protected function getTypeDescription($object) {
		$name = isset($object['name']) ? $object['name'] : null;

		if ($name !== null && isset($this->predefinedPropertyDefList[$name])) {
			// build predefined datatype
			$storageTypeDef = $this->predefinedPropertyDefList[$name];
			$objectDef = [
				'id' => [
					'name' => 'id',
					'description' => 'PK',
					'displaytype' => 'HIDDEN',
					'storagetype' => 'pk',
				],
				'sys_uuid' => [
					'name' => 'sys_uuid',
					'description' => '',
					'displaytype' => 'HIDDEN',
					'storagetype' => 'string',
				],
				'name' => [
					'name' => 'name',
					'description' => $this->app->langTree->getByPath('module_properties.table.name.rus'),
					'displaytype' => 'TEXT',
					'storagetype' => 'string',
				],
				'value' => [
					'name' => 'value',
					'description' => $this->app->langTree->getByPath('module_properties.table.value.rus'),
					'displaytype' => $storageTypeDef['displaytype'],
					'storagetype' => $storageTypeDef['storagetype'],
				],
				'description' => [
					'name' => 'description',
					'description' => $this->app->langTree->getByPath('module_properties.table.description.rus'),
					'displaytype' => 'TEXT|500|100',
					'storagetype' => 'string',
				],
			];


		} else {
			// build default datatype
			$objectDef = [
				'id' => [
					'name' => 'id',
					'description' => 'PK',
					'displaytype' => 'HIDDEN',
					'storagetype' => 'pk',
				],
				'sys_uuid' => [
					'name' => 'sys_uuid',
					'description' => '',
					'displaytype' => 'HIDDEN',
					'storagetype' => 'string',
				],
				'name' => [
					'name' => 'name',
					'description' => $this->app->langTree->getByPath('module_properties.table.name.rus'),
					'displaytype' => 'TEXT',
					'storagetype' => 'string',
				],
				'value' => [
					'name' => 'value',
					'description' => $this->app->langTree->getByPath('module_properties.table.value.rus'),
					'displaytype' => 'TEXT',
					'storagetype' => 'string',
				],
				'description' => [
					'name' => 'description',
					'description' => $this->app->langTree->getByPath('module_properties.table.description.rus'),
					'displaytype' => 'TEXT|500|100',
					'storagetype' => 'string',
				],
			];
		}

		return $this->buildTypeFromArray($objectDef);
	}

	/**
	 * @param $objectDef
	 * @return PXTypeDescription
	 */
	protected function buildTypeFromArray($objectDef) {
		$typeDescription = new PXTypeDescription();
		$typeDescription->id = 'sys_property';
		$typeDescription->title = 'Параметр';

		foreach ($objectDef as $dataDef) {
			$_tmpSource = null;
			if (isset($dataDef['source'], $this->app->directory[$dataDef['source']])) {
				$_tmpSource = $dataDef['source'];
				unset($dataDef['source']);
			}

			$field = new \PXFieldDescription(
				$this->createAttributeNode($dataDef),
				$this->app,
				$typeDescription
			);

			$field->listed = false;
			if ($_tmpSource !== null) {
				$field->source = $_tmpSource;
				$field->values = &$this->app->directory[$_tmpSource];
			}

			$typeDescription->addField($field);
			$typeDescription->assignToGroup($field);
		}

		return $typeDescription;
	}

	/**
	 * @param array $data
	 * @return SimpleXmlNode
	 */
	protected function createAttributeNode($data) {
		$attr = new \SimpleXMLElement("<attribute/>");
		foreach ($data as $k => $v) {
			$attr->addAttribute($k, $v);
		}

		return new SimpleXmlNode($attr);
	}

}
