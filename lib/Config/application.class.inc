<?php

use PP\ConfigurationLocator;
use PP\Contracts\IApplication;
use PP\Lib\ArrayCollection;
use PP\Lib\Config\Description\InitializableDescriptionInterface;
use PP\Lib\Engine\AbstractEngine;
use PP\Lib\Engine\EngineInterface;
use PP\Lib\Xml\Xml;
use PP\Lib\Xml\XmlNodeInterface;
use PP\Properties\EnvLoader;
use PP\Properties\PropertyLoader;
use Symfony\Component\Config\FileLocator;
use Symfony\Component\Yaml\Yaml;

require_once PPLIBPATH . 'Config/Description/db.class.inc';

require_once PPLIBPATH . 'Config/Description/type.class.inc';
require_once PPLIBPATH . 'Config/Description/field.class.inc';

require_once PPLIBPATH . 'Config/Description/directory.class.inc';
require_once PPLIBPATH . 'Config/Description/ref.class.inc';

require_once PPLIBPATH . 'Config/Description/module.class.inc';
require_once PPLIBPATH . 'Config/bindingsQueue.class.inc';

require_once PPLIBPATH . 'Config/Description/trigger.class.inc';
require_once PPLIBPATH . 'Config/triggersList.class.inc';

require_once PPLIBPATH . 'Config/Description/plugin.class.inc';

/**
 * PXApplication - главный класс.
 *
 * @subpackage Application
 */
final class PXApplication implements IApplication
{

	/**
	 * массив коннекторов к базе данных
	 * @var NLDBDescription[]
	 */
	public $dbDescription;

	/**
	 * массив описаний datatypes {@link PXTypeDescription}
	 * @var PXTypeDescription[]
	 */
	public $types = [];

	/**
	 * массив ссылок между datatypes {@link PXRefDescription}
	 * @var PXRefDescription[]
	 */
	public $references;

	/**
	 * контейнер c триггерами {@link PXTriggersList}
	 * @var PXTriggersList[]
	 */
	public $triggers = null;

	/**
	 * массив модулей {@link PXModuleDescription}
	 * @var PXModuleDescription[]
	 */
	public $modules;

	/**
	 * массив плагинов
	 * @var PXPluginDescription[]
	 */
	public $plugins = null;

	/**
	 * Объект настроек
	 *
	 * @var ArrayCollection
	 */
	protected $properties;

	/**
	 * TODO: refactor to protected
	 * массив языковых настроек из (site|local|libpp)/etc/lang.yaml (суммарный)
	 *
	 * @var ArrayCollection
	 */
	public $langTree;

	/**
	 * массив правил авторизации из etc/auth.ini
	 * @var array
	 */
	public $authrules = [];

	/**
	 * массив описаний каталогов из etc/datatypes.xml
	 * @var array
	 */
	public $directory = [];

	/**
	 * биндинги модулей из etc/modules.xml modules/bindings
	 * @var PXBindingDescription[]
	 */
	public $bindingsQueue;

	/**
	 * массив названий полей, чьи объекты требуют инициализации при загрузке из кэша.
	 * Description объекта должен реализовывать метод initialize
	 *
	 * @var array
	 **/
	protected $reinitialize = [];

	/**
	 * @var ConfigurationLocator
	 */
	protected $locator;

	/**
	 * @var int
	 */
	protected $created;

	/**
	 * @var array
	 */
	protected $configurationPaths = [
		APPPATH . 'config/',
		BASEPATH . '/site/etc/',
		BASEPATH . '/local/etc/',
		BASEPATH . '/libpp/etc/',
	];

	/**
	 * Конструктор класса
	 *
	 */
	public function __construct()
	{
		// inject variables from .env into $_ENV
		EnvLoader::inject();

		$this->langTree = new ArrayCollection();
		$this->properties = new ArrayCollection(); // properties will be filled via call in PXAbstractEngine
		$this->locator = new ConfigurationLocator(new FileLocator($this->configurationPaths));
		$this->created = time();
	}

	public function init()
	{
		$this->fillDbDescription();

		$this->fillTriggers($this->locator->locateQuiet('triggers.xml'));
		$this->fillPlugins($this->locator->locateQuiet('plugins.xml'));
		$this->fillModules($this->locator->locate('modules.yml'));

		$dataTypes = $this->locator->locate('datatypes.xml');
		$this->fillDataTypes($dataTypes);
		$this->fillDirectories($dataTypes);

		$this->fillLang($this->locator->locate('lang.yaml', false));
		$this->fillAuthRules($this->locator->locate('auth.ini', false));
	}

	/**
	 * @return int
	 */
	public function getCreated()
	{
		return $this->created;
	}

	/**
	 * @return array
	 */
	public function getConfigurationPaths()
	{
		return array_filter($this->configurationPaths, 'file_exists');
	}

	/**
	 * Load and merge different yaml files
	 *
	 * @param string[] $fileList
	 */
	private function fillLang($fileList)
	{
		$fileList = array_reverse($fileList);
		$langList = array_map(fn($file) => Yaml::parse(file_get_contents($file)), $fileList);

		foreach ($langList as $data) {
			$this->appendLang($data);
		}
	}

	/**
	 * @param array $langArray lang data array to be merged into base one
	 */
	public function appendLang($langArray)
	{
		$langArrayCurrent = $this->langTree->toArray();
		$langArrayCurrent = array_recursive_inheritance($langArrayCurrent, $langArray);
		$this->langTree->fromArray($langArrayCurrent);
	}

	/**
	 * Создает описание подключения к базе данных
	 * @see NLDBDescription, $dbDescription
	 *
	 */
	private function fillDbDescription()
	{
		$this->dbDescription = [
			'database' => NLDBDescription::fromEnv(),
		];
	}

	/**
	 * @param string[] $files
	 */
	private function fillAuthRules($files)
	{
		$this->authrules = self::parseIniFiles($files);
	}

	/**
	 * Возвращает элемент из ({@link properties}) по ключу или значение по умолчанию.
	 *
	 * @param string $k ключ
	 * @param string $defaultValue - возвращаемое значение по умолчанию
	 * @return mixed
	 */
	public function getProperty($k, $defaultValue = null)
	{
		return $this->properties[$k] ?? $defaultValue;
	}

	/**
	 * Возвращает ассоциативный массив доступных модулей.
	 *
	 * @return array         массив из элементов 'module_name' => $module
	 */
	public function getAvailableModules()
	{
		return array_filter($this->modules, fn($module) => PXRegistry::getUser()->can('admin', $module));
	}

	/**
	 * Возвращает модуль по его имени с проверкой доступа к нему.
	 *
	 * @return PXModuleDescription модуль
	 */
	public function getAvailableModule($module_name)
	{
		return isset($this->modules[$module_name]) && PXRegistry::getUser()->can('admin', $this->modules[$module_name]) ? $this->modules[$module_name] : null;
	}

	/**
	 * Возвращает ассоциативный массив доступных модулей.
	 *
	 * @param String $package путь пакаджа (например, "csv/import")
	 * @return array          массив из элементов 'module_name' => $module
	 */
	public function getAvailableModulesWithinPackage($package)
	{
		$availableModules = $this->getAvailableModules();
		if (empty($package)) {
			return $availableModules;
		}

		return array_filter($availableModules, fn($module) => str_starts_with((string) $module->package, $package));
	}

	/**
	 * Возвращает ассоциативный массив разрешенных детей.
	 *
	 * @param NULL $formatName не используется
	 * @param Object|array $object объект для которого искать детей
	 * @return array                массив разрешенных детей или пустой массив
	 */
	public function getAllowedChilds($formatName, &$object)
	{
		if (!isset($this->types[$formatName])) {
			return [];
		}

		return $this->types[$formatName]->allowedChilds($object);
	}

	/**
	 * Возвращает массив наименований разрешенных форматов дочерних объектов.
	 *
	 * @param string $formatName не используется
	 * @param array $object объект для которого искать детей
	 * @return array        массив наименований разрешенных детей или пустой массив
	 * @uses getAllowedChilds
	 *
	 */
	public function getAllowedChildsKeys($formatName, &$object)
	{
		return array_keys($this->getAllowedChilds($formatName, $object));
	}

	/**
	 * @param string $formatName
	 * @return array
	 */
	public function initContentObject(string $formatName): array
	{
		$object = [];

		foreach ($this->types[$formatName]->fields as $k => $v) {
			$object[$k] = $v->defaultValue;
		}

		return $object;
	}

	/**
	 * @param $formatName
	 * @return array
	 * @deprecated will be removed
	 */
	public function initObject($formatName)
	{
		return $this->initContentObject($formatName);
	}

	/**
	 * Заполняет {@link PXApplication::$types} и {@link PXApplication::$references} данными из файла.
	 *
	 * В качестве параметра обычно используется файл etc/datatypes.xml. <br>
	 * XPath /model/datatypes/datatype и /model/references/reference.
	 *
	 * @param String $fileName имя файла
	 * @uses PXDirectoryDescription
	 * @uses PXRefDescription
	 * @uses PXTypeDescription
	 * @uses PXFieldDescription
	 */
	private function fillDataTypes($fileName)
	{
		if (!($dom = Xml::load($fileName))) {
			FatalError("Ошибка в файле настроек <EM>$fileName</EM>");
		}

		PXTypeDescription::fillAppTypes($dom->xpath("/model/datatypes/datatype"), $this);
		PXRefDescription::fillReferences($dom->xpath("/model/references/reference"), $this);
	}

	/**
	 * Заполняет {@link PXApplication::$directory} данными из файла.
	 *
	 * В качестве параметра обычно используется файл etc/datatypes.xml. <br>
	 * XPath /model/directories/directory.
	 *
	 * @param String $fileName имя файла
	 * @uses PXDirectoryDescription
	 *
	 */
	private function fillDirectories($fileName)
	{
		if (!($dom = Xml::load($fileName))) {
			FatalError("Ошибка в файле настроек <EM>$fileName</EM>");
		}

		$domDirectories = $dom->xpath("/model/directories/directory");

		foreach ($domDirectories as $domDirectory) {
			$name = $domDirectory->getAttribute('name');

			if (!isset($this->directory[$name])) {
				$this->directory[$name] = new PXDirectoryDescription($name);
			}

			$curDir =& $this->directory[$name];

			$curDir->load = $domDirectory->getAttribute('load');
			$curDir->schema = $domDirectory->getAttribute('schema');
			$curDir->source = $domDirectory->getAttribute('source');
			$curDir->sourceDir = dirname($fileName) . DIRECTORY_SEPARATOR;

			$displayfield = trim((string) $domDirectory->getAttribute('displayfield'));

			$curDir->displayField = strlen($displayfield) ? $displayfield : 'title';

			$location = $domDirectory->getAttribute('location');
			$curDir->location = ($location != '') ? $location : NULL;

			$sourcekeyfield = trim((string) $domDirectory->getAttribute('source-key-field'));
			$curDir->sourceKeyField = strlen($sourcekeyfield) ? $sourcekeyfield : '@id';

			$sourcedisplayfield = trim((string) $domDirectory->getAttribute('source-display-field'));
			$curDir->sourceDisplayField = strlen($sourcedisplayfield) ? $sourcedisplayfield : '.';

			$sourceitem = trim((string) $domDirectory->getAttribute('source-item'));
			$curDir->sourceItem = strlen($sourceitem) ? $sourceitem : './*';

			$sourcedatafields = trim((string) $domDirectory->getAttribute('source-data-fields'));
			$curDir->sourceDataFields = strlen($sourcedatafields) ? $sourcedatafields : '@*[local-name() != "id"]';

			if ($curDir->load == 'automatic' && $curDir->schema == 'xml') {
				$this->LoadXMLDirectory($curDir, false);
			}
		}
	}

	/**
  * Load properties from database.
  */
 public function loadProperties(PXDatabase $database)
	{
		$propertyList = PropertyLoader::getPropertyList($database);
		$this->properties->fromArray($propertyList);
	}

	/**
	 * Loads a directory by link.
	 * Uses {@link PXDatabase::_GetData()} to load directories from database.
	 *
	 * @param PXDirectoryDescription $directory
	 * @param bool $addDefault
	 */
	public function LoadXMLDirectory(&$directory, $addDefault = true)
	{
		$file = $this->locator->locate($directory->source);
		$xpathSelector = $directory->location;

		$idField = $directory->sourceKeyField;
		$titleField = $directory->sourceDisplayField;
		$xitem = $directory->sourceItem;
		$dataFields = $directory->sourceDataFields;

		if (!($dom2 = Xml::load($file))) {
			FatalError("Ошибка в файле справочника <EM>$file</EM>");
		}

		$results = $dom2->xpath($xpathSelector);

		if ($results) {

			/** @var XmlNodeInterface $result */
			foreach ($results as $result) {

				$nodes = $result->xpath($xitem);
				if (empty($nodes)) {
					continue;
				}

				/** @var XmlNodeInterface $node */
				foreach ($nodes as $node) {
					$id = $node->nodeXValue($idField);

					$directory->values[$id] = [
						'id' => $id,
						'title' => pp_simplexml_decode_string($node->nodeXValue($titleField)),
					];

					$data = $node->xpath($dataFields);

					// Structure attributes fix
					foreach ($data as $dataNode) {
						$key = $dataNode->nodeName();
						if ($dataNode->nodeType() == Xml::ATTRIBUTE) {
							$directory->values[$id][$key] = pp_simplexml_decode_string($dataNode->nodeValue());
							continue;
						}

						$item = [];
						foreach ($dataNode->attributes() as $val) {
							$item[$val->name] = pp_simplexml_decode_string($val->value);
						}
						foreach ($dataNode->childNodes() as $val) {
							$item[$val->nodeName()] = pp_simplexml_decode_string($val->nodeValue());
						}

						empty($directory->values[$id][$key]) && ($directory->values[$id][$key] = []);
						$directory->values[$id][$key][] = $item;
					}
				}

			}

			$directory->SetLoaded();
		}

		$directory->SetLoaded($addDefault);
	}

	private function fillModules($fileName)
	{
		$modules = Yaml::parse(file_get_contents($fileName));

		$modulesList = getFromArray($modules, 'modules', []);
		if (!is_array($modulesList) || count($modulesList) == 0) {
			FatalError('Не определены модули проекта');
		}

		foreach ($modulesList as $moduleName => $module) {
			$description = (new PXModuleDescription())
				->setName($moduleName)
				->setDescription($module['description'])
				->setClass($module['class'])
				->setSettings(getFromArray($module, 'params', []));

			$this->setModules($description);
		}

		$this->bindingsQueue = new PXBindingsQueue();

		$modulesBindings = getFromArray($modules, 'bindings', []);
		foreach ($modulesBindings as $bindingType => $bindings) {
			foreach ($bindings as $binding) {
				$this->bindingsQueue->add($binding, $bindingType);
			}
		}

		$this->bindingsQueue->sort();
	}

	public function setModules(PXModuleDescription $module)
	{
		$this->modules[$module->getName()] = $module;
	}

	/**
	 * Пользовательская функция для регистрации модулей не через конфиг modules.yml, а при загрузке плагинов
	 * @param string $name - имя
	 * @param array $args - массив с параметрами триггера, имя => значение
	 * @return bool
	 * @throws Exception
	 */
	public function registerModule($name, array $args = [])
	{
		if (!ctype_alnum($name)) {
			throw new Exception('Invalid module name');
		}

		$lcname = strtolower($name);
		$ucfname = ucfirst($name);

		if (isset($this->modules[$lcname])) {
			return false;
		}

		$module = (new PXModuleDescription())
			->setName($lcname)
			->setDescription($args['description'] ?? $ucfname)
			->setClass($args['class'] ?? 'PXModule' . $ucfname);

		$this->setModules($module);

		return true;
	}

	/**
	 * Load plugins.
	 *
	 * @param string $fileName
	 */
	private function fillPlugins($fileName)
	{
		if (empty($fileName)) {
			return;
		}

		if (!($dom = Xml::load($fileName))) {
			FatalError("Ошибка в файле настроек <em>$fileName</em>");
		}

		$domPlugins = $dom->xpath("/plugins//plugin");
		if (!is_array($domPlugins) || count($domPlugins) == 0) {
			return;
		}

		foreach ($domPlugins as $domPlugin) {
			$plugin = new PXPluginDescription();
			$plugin->setAttrs($domPlugin->attributes());

			if ($domPlugin->param) {
				$plugin->setParams($domPlugin->param);
			}

			if ($domPlugin->modules) {
				$plugin->setModules($domPlugin->modules);
			}

			if ($domPlugin->triggers) {
				$plugin->setTriggers($domPlugin->triggers);
			}

			$this->plugins[$plugin->getName()] = $plugin;
			$plugin->getPlugin($this); // initialize plugin
		}

		$this->reinitialize['plugins'] = true;
	}

	/**
	 * Заполняет {@link PXApplication::$triggers} данными из файла.
	 *
	 * В качестве параметра обычно используется файл etc/triggers.xml. <br>
	 * XPath /triggers//trigger
	 * Инициализирует поле triggers объектом-коллекцией триггеров
	 *
	 * @param String $fileName имя файла
	 * @uses PXTriggerDescription
	 *
	 */
	private function fillTriggers($fileName)
	{
		$this->triggers = new PXTriggersList();

		if (empty($fileName)) {
			return;
		}

		if (!($dom = Xml::load($fileName))) {
			FatalError("Ошибка в файле настроек <em>$fileName</em>");
		}

		$domTriggers = $dom->xpath('/triggers//trigger');

		if (!is_array($domTriggers) || count($domTriggers) == 0) {
			FatalError('Не описаны триггеры проекта');
		}

		foreach ($domTriggers as $domTrigger) {
			$this->setTrigger($domTrigger->attributes(), $domTrigger->parent()->nodeName());
		}
	}

	private function setTrigger($attributes, $type)
	{
		$trigger = new PXTriggerDescription();
		$trigger->setAttrs($attributes);
		$trigger->setType($type);

		if (!isset($this->triggers[$type])) {
			$this->triggers[$type] = [];
		}

		$this->triggers[$type][$trigger->getName()] = $trigger;
	}

	/**
	 * Пользовательская функция для регистрации триггеров не через конфиг triggers.xml, а в процессе работы системы
	 *
	 * @param string $type - тип триггера
	 * @param array $args - массив с параметрами триггера, имя => значение
	 * @return $this
	 */
	public function registerTrigger($type, array $args)
	{
		foreach ($args as $name => &$value) {
			$value = Xml::attributePrototype($name, $value); //XML style attributes compatibility
		}

		$this->setTrigger($args, $type);
		return $this;
	}

	private static function parseIniFile($file)
	{
		return (array)parse_ini_file($file, true);
	}

	private static function parseIniFiles($files)
	{
		/* we must merge array in reverse order: libpp + local + site,
		 * so local can redefine libpp settings, and site can override local */

		$result = self::parseIniFile(array_pop($files));

		while (count($files)) {
			$tmp = self::parseIniFile(array_pop($files));

			foreach ($tmp as $k => $v) {
				if (is_array($v) and isset($result[$k]) and is_array($result[$k])) {
					$result[$k] = array_merge($result[$k], $v);
				} else {
					$result[$k] = $v;
				}
			}
		}

		return $result;
	}

	/**
	 * Return if application is in development mode
	 * @return bool
	 */
	public function isDevelopmentMode()
	{
		return $this->getProperty('ENVIRONMENT') === ENV_DEVELOPER;
	}

	/**
	 * @return bool
	 */
	public function isAdminEngine()
	{
		return PXRegistry::getEngine()->engineClass() === EngineInterface::ADMIN_ENGINE_ID;
	}

	/**
	 * @param string $engineName
	 * @return bool
	 */
	public function isEngineInstanceof($engineName)
	{
		$engineName = 'PxEngine' . $engineName;
		return PXRegistry::getEngine() instanceof $engineName;
	}

	/**
	 * @param $name
	 * @return null|AbstractEngine
	 */
	public function __get($name)
	{
		if (strtolower((string) $name) === 'engine') {
			return PXRegistry::getEngine();
		}

		return null;
	}

	/**
	 * @throws Exception
	 */
	public function __wakeup()
	{
		EnvLoader::inject();

		foreach ($this->reinitialize as $name => $bool) {
			foreach ((array)$this->{$name} as $o) {
				if (!$o instanceof InitializableDescriptionInterface) {
					throw new Exception(
						sprintf(
							"Class: %s is not instanceof InitializableDescriptionInterface",
							$o::class
						)
					);
				}

				$o->initialize($this);
			}
		}
	}
}
