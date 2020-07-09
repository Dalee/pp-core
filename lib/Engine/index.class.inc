<?php

use Symfony\Component\DependencyInjection\ContainerAwareInterface;
use PP\Lib\Engine\AbstractEngine;
use PP\Lib\Html\Layout\LayoutInterface;
use PP\Lib\Html\Layout\NullLayout;

class PXEngineIndex extends AbstractEngine {
	var $layout = ['factory' => LayoutInterface::class, 'helper' => true];

	/**
	 * Инициализация основных объектов приложения
	 * Initializes application's main objects
	 */
	public function __construct() {
		parent::__construct();

		$this->content             = new StdClass; // FIXME
		$this->content->tree       = new PXTreeObjects();
		$this->content->objects    = new PXObjects();
		$this->content->subObjects = new PXSubObjects();
	}

	protected function initModules() {
	}

	protected function initLayout($layoutClass) {
		if (!$this->container->has($layoutClass)) {
			$this->layout = new NullLayout();

			return;
		}

		// TODO: Suppose init lang must be optional and must be done using container.
		// TODO: Binding lang to layout is rather questionable too, because it's often needed in userAction/userJson.
		$layout = $this->container->get($layoutClass);
		$lang = new PXUserHTMLLang();
		$lang->setTree($this->app->langTree);
		$layout->setLang($lang);

		$this->layout = $layout;
	}

	public function fillLayout() {
		$this->layout->assign('app', PXRegistry::getApp());
		$this->layout->assign('user', PXRegistry::getUser());
		$this->layout->assign('request', PXRegistry::getRequest());
		$this->layout->assign('response', PXResponse::getInstance());
		$this->layout->assign('tree', $this->content->tree);
		$this->layout->assign('rootContent', $this->content->tree->root->content);
		$this->layout->assign('currentStructContent', $this->content->tree->current->content);
		$this->layout->assign('objects', $this->content->objects);
		$this->layout->assign('subObjects', $this->content->subObjects);
	}

	public function html() {
		$html = PXRegistry::getLayout()->display();

		$response = PXResponse::getInstance();
		$response->send($html);
	}

	public function runModules() {
		$queue = PXRegistry::getApp()->bindingsQueue;
		$queue->getRequestBinding();

		foreach($queue as $bind) {
			if(!$bind->isBinding($this->content)) {
				continue;
			}

			$moduleDescription = $bind->getModuleDescription();
			$instance = $moduleDescription->getModule();

			if ($instance instanceof ContainerAwareInterface) {
				$instance->setContainer($this->container);
			}

			$instance->tree       =& $this->content->tree;
			$instance->objects    =& $this->content->objects;
			$instance->subObjects =& $this->content->subObjects;

			$eventData = [
				'engine_type' => $this->engineType(),
				'engine_behavior' => $this->engineBehavior()
			];
			foreach ($this->app->triggers->system as $t) {
				$t->getTrigger()->onBeforeModuleRun($this, $moduleDescription, $eventData);
			}

			$instance->userIndex();

			foreach ($this->app->triggers->system as $t) {
				$t->getTrigger()->onAfterModuleRun($this, $moduleDescription, $eventData);
			}
		}
	}

	/** {@inheritdoc} */
	public function engineType() {
		return static::USER_ENGINE_TAG;
	}

	/** {@inheritdoc} */
	public function engineBehavior() {
		return static::INDEX_BEHAVIOR;
	}
}

