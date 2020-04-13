<?php

namespace PP\Module;

use PP\Lib\UrlGenerator\ContextUrlGenerator;
use PP\Lib\UrlGenerator\UrlGenerator;
use PP\Properties\PropertyLoader;
use Ramsey\Uuid\Uuid;
use PP\Lib\Xml\SimpleXmlNode;

/**
 * Class PropertiesModule.
 *
 * @see libpp/docs/properties.module.md
 * @package PP\Module
 */
class PropertiesModule extends AbstractModule
{

	/** @var array */
	protected $predefinedPropertyDefList = [];

	/**
	 * {@inheritdoc}
	 */
	public function __construct($area, $settings, $selfDescription)
	{
		parent::__construct($area, $settings, $selfDescription);

		if (!empty($this->settings['attribute'])) {
			$this->parsePredefinedProperties((array)$this->settings['attribute']);
		}
	}

	/**
	 * {@inheritdoc}
	 */
	public static function getAclModuleActions()
	{
		$defaults = parent::getAclModuleActions();
		$defaults['sys_properties_edit'] = \PXRegistry::getApp()
			->langTree
			->getByPath('module_acl_rules.actions.sys_properties_edit.rus');

		return $defaults;
	}

	/**
	 * {@inheritdoc}
	 */
	public function adminIndex()
	{
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

		$context = new ContextUrlGenerator();
		$context->setCurrentModule($this->area);
		$generator = new UrlGenerator($context);
		$adminGenerator = $generator->getAdminGenerator();

		$popupParams = [
			'action' => 'edit',
			'id' => '',
		];

		$table = (new \PXAdminTableSimple($colsDef))
			->setTableId('properties')
			->showEven()
			->setNullText($this->app->langTree->getByPath('module_properties.table_ctrl.empty_value.rus'))
			->setData($this->getPropertyList($sid))
			->setControls(
				$adminGenerator->popupUrl($popupParams),
				$this->app->langTree->getByPath('module_properties.table_ctrl.edit.rus'),
				'edit',
				false,
				true
			);

		if ($this->isPowerUser()) {
			$actionParams = [
				'action' => 'delete',
				'id' => '',
			];
			$table->setControls(
				$adminGenerator->actionUrl($actionParams),
				$this->app->langTree->getByPath('module_properties.table_ctrl.delete.rus'),
				'del',
				true,
				false
			);
			$table->setDict('value', function ($row, $val) {
				if (!is_numeric($row['id'])) {
					return $val;
				}
				$typedef = $this->getTypeDescription($row);
				if (isset($typedef->fields['value']->displayType)) {
					$typedef->fields[$pseudoName = "properties[{$row['id']}]"] = $typedef->fields['value'];
					$row[$pseudoName] = $val ?? '';
					$typedef->fields[$pseudoName]->name = $pseudoName;
					$control = '';
					if ($typedef->fields['value']->displayType instanceof \PXDisplayTypeCheckbox) {
						$control .= sprintf(
							'<input type="hidden" name="properties_unchecked[%s]" value="1">', $row['id']);
					}
					$control .= $typedef->fields[$pseudoName]->displayType->buildInput(
						$typedef->fields[$pseudoName], $row);
					return $control;
				}
				return $val;
			});
			$this->layout->append('INNER.1.0',
				sprintf('<form class="simpleform" method="POST" action="%s">', $adminGenerator->actionUrl()));

			$this->layout->assignInlineCSS('.simpleform td input, .simpleform td textarea { width: 100% }');
		}

		$table->addToParent('INNER.1.0');

		// build additional controls
		if ($this->isPowerUser()) {
			$saveAllButtonName = $this->app->langTree->getByPath('module_properties.table_ctrl.save_all.rus');
			$this->layout->append('INNER.1.0', <<<MASS_ACTIONS
				<p class="field">
					<button class="add" name="action" value="save_all">{$saveAllButtonName}</button>
				</p>
MASS_ACTIONS
			);

			$link = sprintf("Popup('%s');", $adminGenerator->popupUrl(['action' => 'main']));
			$label = $this->app->langTree->getByPath('module_properties.table_ctrl.add.rus');

			$button = (new \PXControlButton($label))
				->setClickCode($link)
				->setClass('add');

			$button->addToParent('INNER.1.1');
		}
	}

	/**
	 * {@inheritdoc}
	 */
	public function adminPopup()
	{
		$id = $this->request->getId();

		$propertyDef = $this->getPropertyDefById($id);
		$typeDef = $this->getTypeDescription($propertyDef);
		$form = new \PXAdminForm(
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

		$this->layout->assign('OUTER.MENU', '');
	}

	/**
	 * {@inheritdoc}
	 */
	public function adminAction()
	{
		$redirect = null;

		switch ($this->request->getAction()) {
			case 'main':
				$id = $this->request->getId();
				$redirect = $this->saveAction($id, $this->getPropertyFromRequest($id));
				break;

			case 'delete':
				$id = $this->request->getId();
				$this->deleteAction($id, $this->getPropertyFromRequest($id));
				break;
			case 'save_all':
				$props = (array)$this->request->getVar('properties');
				$props_unchecked = (array)$this->request->getVar('properties_unchecked');

				foreach ($props_unchecked as $id => $v) {
					if (!array_key_exists($id, $props)) {
						$props[$id] = null;
					}
				}

				if (!count($props)) {
					return null;
				}

				foreach ($props as $id => $value) {
					$this->request->setVar('value', $value);
					$prop = $this->getPropertyFromRequest($id);
					$this->saveAction($id, ['value' => $prop['value']]);
				}

				break;
		}

		return $redirect;
	}

	protected function getPropertyFromRequest($id)
	{
		$propertyDef = $this->getPropertyDefById($id);
		$typeDef = $this->getTypeDescription($propertyDef);
		return $this->request->getContentObject($typeDef);
	}

	/**
	 * @param $id
	 * @return array|null
	 */
	protected function getPropertyDefById($id)
	{
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

		// protect from non-power users..
		if (!$this->isPowerUser() && $this->isPropertySystem($propertyDef)) {
			FatalError("Access denied");
		}

		return $propertyDef;
	}

	/**
	 * Delete property by id.
	 * Public defined properties will be displayed anyway.
	 *
	 * @param mixed $id
	 * @param array $object
	 */
	protected function deleteAction($id, $object)
	{
		$id = is_numeric($id) ? $id : null;
		if (empty($id)) {
			return;
		}

		$deleteQuery = sprintf("DELETE FROM %s WHERE id=%d", DT_PROPERTIES, $this->db->EscapeString($id));
		$this->db->ModifyingQuery($deleteQuery, DT_PROPERTIES);
	}

	/**
	 * Update/Insert new property to database.
	 * Only power users can create new properties.
	 *
	 * @param mixed $id
	 * @param array $object
	 * @return string
	 */
	protected function saveAction($id, $object)
	{
		unset($object['id']);
		if (empty($object['sys_uuid'])) {
			$object['sys_uuid'] = Uuid::uuid4()->toString();
		}

		$fields = array_keys($object);
		$values = array_values($object);
		$id = is_numeric($id) ? $id : null;

		if (empty($id)) {
			$id = $this->db->InsertObject(DT_PROPERTIES, $fields, $values);
		} else {
			$this->db->UpdateObjectById(DT_PROPERTIES, $id, $fields, $values);
		}

		$context = new ContextUrlGenerator();
		$context->setCurrentModule($this->area);
		$generator = new UrlGenerator($context);

		$popupParams = [
			'action' => 'main',
			'id' => $id,
		];

		$redirect = $generator->getAdminGenerator()->popupUrl($popupParams);

		return $redirect;
	}

	/**
	 * @param array $publicProperties
	 */
	protected function parsePredefinedProperties(array $publicProperties)
	{
		foreach ($publicProperties as $propertyStrDef) {
			$propertyDef = [];
			$formatString = str_replace('|', '&', $propertyStrDef);

			parseStrMagic($formatString, $propertyDef);
			if (!isset($propertyDef['name'])) {
				FatalError('В параметрах модуля, для одного из полей отсутствует обязательный параметр name');
			}

			$propertyDef['description'] = isset($propertyDef['description'])
				? pp_simplexml_encode_string($propertyDef['description'])
				: $propertyDef['name'];

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
	protected function isPowerUser()
	{
		return $this->user->can('sys_properties_edit', $this->app->modules[$this->area]);
	}

	/**
	 * @param array $propertyRaw
	 * @return bool
	 */
	protected function isPropertySystem(array $propertyRaw)
	{
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
	protected function getPropertyList($sid)
	{
		$propertyList = PropertyLoader::getRawPropertyList($this->db);

		// filtering callable
		$propertySidFilter = function ($propertyDef) use (&$sid) {
			if ($sid === 'pub') {
				return $this->isPropertySystem($propertyDef) === false;
			} else {
				return $this->isPropertySystem($propertyDef);
			}
		};

		// filter property list based on sid
		$propertyList = array_filter($propertyList, $propertySidFilter);

		// fetch and filter defined settings
		$propertyListDef = array_filter($this->predefinedPropertyDefList, $propertySidFilter);

		// extract properties already present in database and defined in module settings
		$createdDef = array_filter($propertyList, function ($propertyRaw) use (&$propertyListDef) {
			return isset($propertyListDef[$propertyRaw['name']]);
		});

		$createdDef = array_flat($createdDef, 'name');
		$missingDef = array_diff_key($propertyListDef, $createdDef);

		// mixin defined, but not yet created properties
		foreach ($missingDef as $propertyDef) {
			$format = $this->getTypeDescription($propertyDef);
			$table = $this->normalizeType($propertyDef, $format);
			$propertyList = array_merge($table, $propertyList);
		}

		// transform and mixin defined and created properties..
		$propertyList = array_map(
			function ($propertyRaw) use (&$createdDef) {
				$key = $propertyRaw['name'];
				if (isset($createdDef[$key])) {
					$propertyDef = $createdDef[$key];
					$format = $this->getTypeDescription($propertyDef);
					$table = $this->normalizeType($propertyDef, $format);
					return reset($table);
				}

				return $propertyRaw;
			},
			$propertyList
		);

		// resort all properties
		usort($propertyList, function ($left, $right) {
			return mb_strcasecmp($left['name'], $right['name']);
		});

		return $propertyList;
	}

	/**
	 * Perform property normalization after database fetch.
	 *
	 * @param array $propertyRaw
	 * @param \PXTypeDescription $format
	 * @return array
	 */
	protected function normalizeType($propertyRaw, $format)
	{
		$table = [
			[
				'id' => isset($propertyRaw['id']) ? $propertyRaw['id'] : $propertyRaw['name'],
				'sys_created' => null,
				'sys_modified' => null,
				'name' => $propertyRaw['name'],
				'description' => $propertyRaw['description'],
				'value' => isset($propertyRaw['value']) ? $propertyRaw['value'] : null,
			],
		];

		$this->db->_NormalizeTable($table, $format, true);
		return $table;
	}

	/**
	 * @param array $object
	 * @return \PXTypeDescription
	 */
	protected function getTypeDescription($object)
	{
		$name = isset($object['name']) ? $object['name'] : null;
		$displayTypeDef = [];

		// build predefined datatype?
		if ($name !== null && isset($this->predefinedPropertyDefList[$name])) {
			$displayTypeDef = $this->predefinedPropertyDefList[$name];
		}

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
				'description' => $this->app->langTree->getByPath('module_properties.table.name.rus'),
				'displaytype' => 'TEXT',
				'storagetype' => 'string',
			],
			'value' => [
				'name' => 'value',
				'description' => $this->app->langTree->getByPath('module_properties.table.value.rus'),
				'displaytype' => isset($displayTypeDef['displaytype']) ? $displayTypeDef['displaytype'] : 'TEXT',
				'storagetype' => 'string',
			],
			'description' => [
				'name' => 'description',
				'description' => $this->app->langTree->getByPath('module_properties.table.description.rus'),
				'displaytype' => 'TEXT|500|100',
				'storagetype' => 'string',
			],
		];

		return $this->buildTypeFromArray($objectDef);
	}

	/**
	 * @param $objectDef
	 * @return \PXTypeDescription
	 */
	protected function buildTypeFromArray($objectDef)
	{
		$typeDescription = new \PXTypeDescription();
		$typeDescription->id = 'sys_property';
		$typeDescription->title = $this->app->langTree->getByPath('module_properties.table.name.rus');

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
	protected function createAttributeNode($data)
	{
		$attr = new \SimpleXMLElement("<attribute/>");
		foreach ($data as $k => $v) {
			$attr->addAttribute($k, $v);
		}

		return new SimpleXmlNode($attr);
	}

}
