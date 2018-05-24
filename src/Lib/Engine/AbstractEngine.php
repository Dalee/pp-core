<?php

namespace PP\Lib\Engine;

use Symfony\Component\Config\FileLocator;
use Symfony\Component\DependencyInjection\Loader\YamlFileLoader;
use PP\Lib\Database\Driver\PostgreSqlDriver;
use PP\Lib\Html\Layout\LayoutInterface;
use PP\ApplicationFactory;
use Symfony\Component\DependencyInjection\Dumper\PhpDumper;
use Symfony\Component\Config\ConfigCache;

abstract class AbstractEngine implements EngineInterface {

	/** @var \PXModuleDescription[] */
	protected $modules;

	/** @var string */
	protected $area;

	/** @var ApplicationFactory */
	protected $app = ['factory' => 'PP\ApplicationFactory', 'helper' => true];

	/** @var \PXRequest */
	protected $request = ['factory' => 'PXRequest'];

	/** @var \PXDatabase|PostgreSqlDriver */
	protected $db = ['factory' => 'PXDatabase', 'helper' => true];

	/** @var LayoutInterface */
	protected $layout = ['factory' => 'PP\Lib\Html\Layout\NullLayout'];

	/** @var \PXUserAuthorized */
	protected $user = ['factory' => 'PXUserAuthorized'];

	/**
	 * @var \Symfony\Component\DependencyInjection\ContainerInterface
	 */
	protected $container = ['factory' => 'Symfony\Component\DependencyInjection\ContainerBuilder', 'helper' => true];

	/**
	 * @var ConfigCache
	 */
	protected $containerConfigCache;

	/**
	 * @var array
	 */
	protected $initOrder = ['container', 'app', 'db', 'request', 'user', 'layout'];

	// TODO: refactor
	function __construct() {
		$this->initApplication();
		$this->saveToRegistry();
	}

	/**
	 * {@inheritdoc}
	 */
	public function start() {
		$this->user->setDb($this->db);
		$this->user->setApp($this->app);
		$this->user->setRequest($this->request);
		$this->user->checkAuth();

		/** @var \PXTriggerDescription $t */
		foreach ($this->app->triggers->system as $t) {

			/** @var \PXAbstractSystemTrigger $trigger */
			$trigger = $t->getTrigger();
			$trigger->onAfterEngineStart($this);
		}

		$this->app->loadProperties($this->db);
		$this->db->loadDirectoriesAutomatic($this->app->directory);

		$this->initModules();

		return $this;
	}

	/**
	 * Initializes DI container.
	 *
	 * @param string $klass
	 * @throws \InvalidArgumentException if services.yml is not found
	 * @throws \Exception
	 */
	protected function initContainer($klass) {
		$file = RUNTIME_PATH . 'container.php';
		$this->containerConfigCache = new ConfigCache($file, false);

		if ($this->containerConfigCache->isFresh()) {
			require_once $file;
			$this->container = new \MyCachedContainer();
			return;
		}

		$path = APPPATH . 'config';
		$container = new $klass();
		$loader = new YamlFileLoader($container, new FileLocator($path));

		$loader->load('services.yml');
		$this->container = $container;
	}

	protected function initApplication() {
		foreach ($this->initOrder as $var) {
			if (!is_array($this->$var) || empty($this->{$var}['factory'])) {
				continue;
			}

			if (!class_exists($this->{$var}['factory'])) {
				\FatalError("Factory class '{$this->{$var}['factory']}' for '{$var}' is not exists !");
			}

			$initHelper = (!empty($this->{$var}['helper']) && method_exists($this, 'init' . ucfirst($var)) ? 'init' . ucfirst($var) : false);

			if ($initHelper) {
				$this->$initHelper($this->{$var}['factory']);
			} else {
				$this->$var = new $this->{$var}['factory'];
			}
		}
	}

	protected function saveToRegistry() {
		foreach (array_keys(get_object_vars($this)) as $var) {
			if (is_object($this->$var) && \PXRegistry::canSaveIt($var)) {
				call_user_func_array(['PXRegistry', 'set' . ucfirst($var)], [&$this->$var]);
			}
		}

		if ($this->containerConfigCache->isFresh()) {
			return;
		}

		$this->container->compile(true);

		$dumper = new PhpDumper($this->container);
		$this->containerConfigCache->write(
			$dumper->dump(['class' => 'MyCachedContainer']),
			$this->container->getResources()
		);
	}

	protected function initApp($klass) {
		$this->app = ApplicationFactory::create($this);
	}

	protected function initDb($klass) {
		$this->db = new $klass($this->app);
	}

	protected function initModules() {
		$rArea = $this->request->getArea();

		if (!isset($this->app->modules[$rArea])) {
			Finalize('/');
		}

		$this->modules = $this->app->modules;
		$this->area = $rArea;
	}

	abstract function runModules();

	public function engineClass() {
		return static::USER_ENGINE_ID;
	}

	protected function checkArea($area) {
		if (!isset($this->modules[$area])) {
			FatalError('Некорректный параметр area = <em>' . strip_tags($this->area) . '</em>');
		}
	}

	public function getContainer() {
		return $this->container;
	}

	/** {@inheritdoc} */
	abstract public function engineType();

	/** {@inheritdoc} */
	abstract public function engineBehavior();
}
