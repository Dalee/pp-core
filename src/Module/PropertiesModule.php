<?php

namespace PP\Module;

use PP\Lib\UrlGenerator\ContextUrlGenerator;
use PP\Lib\UrlGenerator\UrlGenerator;
use PP\Properties\PropertyLoader;
use PP\Properties\PropertyTypeBuilder;
use PXAuditLogger;
use Ramsey\Uuid\Uuid;

/**
 * Class PropertiesModule.
 *
 * @see libpp/docs/properties.module.md
 * @package PP\Module
 */
class PropertiesModule extends AbstractModule
{
    protected array $predefinedPropertyDefList = [];

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
            ->getByPath('module_macl_rules.actions.sys_properties_edit.rus');

        return $defaults;
    }

    /**
     * {@inheritdoc}
     */
    public function adminIndex()
    {
        // get sid
        $sid = $this->request->getSid();
        $sid = $sid ?: 'pub';
        $sid = $this->isPowerUser() ? $sid : 'pub';

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
                            '<input type="hidden" name="properties_unchecked[%s]" value="1">',
                            $row['id']
                        );
                    }
                    $control .= $typedef->fields[$pseudoName]->displayType->buildInput(
                        $typedef->fields[$pseudoName],
                        $row
                    );
                    return $control;
                }
                return $val;
            });
            $this->layout->append(
                'INNER.1.0',
                sprintf('<form class="simpleform" method="POST" action="%s">', $adminGenerator->actionUrl())
            );

            $this->layout->assignInlineCSS('.simpleform td input, .simpleform td textarea { width: 100% }');
        }

        $table->addToParent('INNER.1.0');

        // build additional controls
        if ($this->isPowerUser()) {
            $saveAllButtonName = $this->app->langTree->getByPath('module_properties.table_ctrl.save_all.rus');
            $this->layout->append(
                'INNER.1.0',
                <<<MASS_ACTIONS
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
            ],
            $typeDef
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
                $id = $this->request->getId() ?: null;
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

				$this->saveAllAction($props);

                break;
        }

        return $redirect;
    }

    protected function getPropertyFromRequest($id, array $preloadPropertyList = [])
    {
        $propertyDef = $this->getPropertyDefById($id, $preloadPropertyList);
        $typeDef = $this->getTypeDescription($propertyDef);
        return $this->request->getContentObject($typeDef);
    }

	/**
	 * @param $id
	 * @param array $preloadPropertyList
	 *
	 * @return array
	 */
    protected function getPropertyDefById($id, array $preloadPropertyList = []): array
	{
        if (isset($this->predefinedPropertyDefList[$id])) {
            $propertyDef = $this->predefinedPropertyDefList[$id];
            $propertyDef['id'] = $propertyDef['name'];
            $propertyDef['sys_uuid'] = '';
            $propertyDef['value'] = '';

        } else {
			$propertyDef = $preloadPropertyList[$id] ?? PropertyLoader::getPropertyById($id, $this->db);

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
			$audit = PXAuditLogger::getLogger();
			$errMessage = sprintf('%s `%s`', 'Отказано в доступе к параметру ', $propertyDef['name']);
			$audit->error($errMessage, $this->getAuditSource($propertyDef['id']));

            FatalError("Access denied");
        }

        return $propertyDef;
    }

    /**
    * Delete property by id.
    * Public defined properties will be displayed anyway.
    *
    * @param array $object
    */
    protected function deleteAction(mixed $id, $object)
    {
        $id = is_numeric($id) ? $id : null;
        if (empty($id)) {
            return;
        }

		$propertyInDB = PropertyLoader::getPropertyById($id, $this->db);

        $deleteQuery = sprintf("DELETE FROM %s WHERE id=%d", DT_PROPERTIES, $this->db->EscapeString($id));
        $countDeleted = $this->db->ModifyingQuery(
			query: $deleteQuery,
			table: DT_PROPERTIES,
			retCount: true
		);

		$audit = PXAuditLogger::getLogger();
		$auditSource = $this->getAuditSource($id);

		if ($countDeleted > 0) {
			$auditMessage = sprintf('%s `%s`', 'Параметр удалён', $propertyInDB['name']);
			$audit->info($auditMessage, $auditSource);
		} else {
			$errMessage = sprintf('%s `%s`', 'Ошибка удаления параметра', $propertyInDB['name']);
			$audit->error($errMessage, $auditSource);
		}
    }

    /**
    * Update/Insert new property to database.
    * Only power users can create new properties.
    *
    * @param array $object
    * @return string
    */
    protected function saveAction(?int $id, array $object): string
    {
		$objectInDb = $id
			? PropertyLoader::getPropertyById($id, $this->db)
			: null;

        $propertyId = $this->saveAfterCompare($id, $object, $objectInDb);

		$context = new ContextUrlGenerator();
		$context->setCurrentModule($this->area);
		$generator = new UrlGenerator($context);

		$popupParams = [
			'action' => 'main',
			'id' => $propertyId,
		];

		return $generator->getAdminGenerator()->popupUrl($popupParams);
    }

	protected function saveAllAction(array $propertyList): void
	{
		$propertyIds = array_keys($propertyList);
		$propertiesInDb = PropertyLoader::getRawPropertyListByIds($this->db, $propertyIds);

		foreach ($propertyList as $id => $value) {
			$this->request->setVar('value', $value);
			$prop = $this->getPropertyFromRequest($id, $propertiesInDb);

			$this->saveAfterCompare($id, ['value' => $prop['value']], $propertiesInDb[$id]);
		}
	}

	protected function saveAfterCompare(?int $id, array $object, ?array $objectInDb = []): ?int
	{
		unset($object['id']);
		if (empty($object['sys_uuid'])) {
			$object['sys_uuid'] = Uuid::uuid4()->toString();
		}

		$fields = array_keys($object);
		$values = array_values($object);

		$audit = PXAuditLogger::getLogger();

		if (empty($id)) {
			$id = $this->db->InsertObject(DT_PROPERTIES, $fields, $values);

			if ($id > 0) {
				$auditMessage = sprintf('%s `%s`', 'Параметр добавлен', $object['name']);
				$audit->info($auditMessage, $this->getAuditSource($id));
			} else {
				$id = null;
				$errMessage = sprintf('%s `%s`', 'Ошибка добавления параметра', $object['name']);
				$audit->error($errMessage, $this->getAuditSource());
			}
		} else {
			unset($object['sys_uuid']);
			$propertyDiff = array_keys(array_diff($object, $objectInDb));

			if (!empty($propertyDiff)) {
				$auditSource = $this->getAuditSource($id);

				$result = $this->db->UpdateObjectById(DT_PROPERTIES, $id, $fields, $values);

				if (!is_numeric($result)) {
					$auditMessage = sprintf('%s `%s`', 'Параметр изменен', $objectInDb['name']);
					$audit->info(
						description: $auditMessage,
						source: $auditSource,
						diff: json_encode($propertyDiff),
					);
				} else {
					$errMessage = sprintf('%s `%s`', 'Ошибка изменения параметра', $objectInDb['name']);
					$audit->error($errMessage, $auditSource);
				}
			}
		}

		return $id;
	}

    protected function parsePredefinedProperties(array $publicProperties)
    {
        foreach ($publicProperties as $propertyStrDef) {
            $propertyDef = [];
            $formatString = str_replace('|', '&', (string) $propertyStrDef);

            parseStrMagic($formatString, $propertyDef);
            if (!isset($propertyDef['name'])) {
                FatalError('В параметрах модуля, для одного из полей отсутствует обязательный параметр name');
            }

            $propertyDef['description'] = isset($propertyDef['description'])
                ? pp_simplexml_encode_string($propertyDef['description'])
                : $propertyDef['name'];

            $propertyDef['displaytype'] = isset($propertyDef['displaytype'])
                ? str_replace(',', '|', (string) $propertyDef['displaytype'])
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
    * @return bool
    */
    protected function isPropertySystem(array $propertyRaw)
    {
        // public property - property listed in module settings and don't have SYS_/sys_ prefix
        $key = $propertyRaw['name'];
        $prefix = mb_strtolower(mb_substr((string) $key, 0, mb_strlen('sys_')));

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
			return $sid === 'sys'
				? $this->isPropertySystem($propertyDef)
				: $this->isPropertySystem($propertyDef) === false;
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
        usort($propertyList, fn ($left, $right) => mb_strcasecmp($left['name'], $right['name']));

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
                'id' => $propertyRaw['id'] ?? $propertyRaw['name'],
                'sys_created' => null,
                'sys_modified' => null,
                'name' => $propertyRaw['name'],
                'description' => $propertyRaw['description'],
                'value' => $propertyRaw['value'] ?? null,
            ],
        ];

        $this->db->_NormalizeTable($table, $format, true);
        return $table;
    }

	/**
	 * @param array $object
	 *
	 * @return \PXTypeDescription
	 * @throws \Exception
	 */
    protected function getTypeDescription($object)
    {
        $name = $object['name'] ?? null;
        $displayTypeDef = [];

        // build predefined datatype?
        if ($name !== null && isset($this->predefinedPropertyDefList[$name])) {
            $displayTypeDef = $this->predefinedPropertyDefList[$name];
        }
		$displayType = $displayTypeDef['displaytype'] ?? 'TEXT';

        return PropertyTypeBuilder::create($this->app, $displayType);
    }

	private function getAuditSource(string|int|null $id = 0): string
	{
		return sprintf('%s/%s', PropertyTypeBuilder::TYPE_ID, (int) $id);
	}
}
