<?php

class PXEngineIndex extends PXEngine {
	var  $layout = array('factory' => 'PXUserHTMLLayout');

	function __construct() {     // Инициализация основных объектов приложения
		parent::profilerStart();
		parent::__construct();

		$this->content             = new StdClass; // FIXME
		$this->content->tree       = new PXTreeObjects();
		$this->content->objects    = new PXObjects();
		$this->content->subObjects = new PXSubObjects();

		$this->layout->setApp($this->app);
	}

	function initModules() {
	}

	function fillLayout() {
		$this->layout->setApp(PXRegistry::getApp());

		// Передача данных в шаблонизатор
		$this->layout->assign('app',            PXRegistry::getApp(),      true);
		$this->layout->assign('user',           PXRegistry::getUser(),     true);
		$this->layout->assign('request',        PXRegistry::getRequest(),  true);
		$this->layout->assign('response',       PXResponse::getInstance(), true);

		$this->layout->assignRequest(PXRegistry::getRequest());

		$this->layout->assign('tree',           $this->content->tree);
		$this->layout->assign('objects',        $this->content->objects);
		$this->layout->assign('subObjects',     $this->content->subObjects);

		// Deprecated features
		$currentSid  = $this->content->tree->hasCurrent() ? $this->content->tree->getCurrent()->id : -1;
		if($this->content->objects->hasCurrent()) {
			$objs = $this->content->objects->getCurrent();
			$currentCid = $objs['id'];
		} else {
			$currentCid = -1;
		}

		if($this->content->subObjects->hasCurrent()) {
			$objs = $this->content->subObjects->getCurrent();
			$currentSCid = $objs['id'];
		} else {
			$currentSCid = -1;
		}

		$this->layout->assign('currentSid',     $currentSid);
		$this->layout->assign('currentCid',     $currentCid);
		$this->layout->assign('currentSCid',    $currentSCid);

		$this->layout->assign('currentCtype',   $this->content->objects->getCurrentType());

		$this->layout->assign('pathId',         $this->content->tree->pathId);
		$this->layout->assign('rootId',         $this->content->tree->rootId);
		$this->layout->assign('urlFile',        PXRegistry::getRequest()->getFile());

		$this->layout->assign('requestHost',    PXRegistry::getRequest()->getHttpHost());
		$this->layout->assign('requestUri',     PXRegistry::getRequest()->getRequestUri());
		$this->layout->assign('requestReferer', PXRegistry::getRequest()->getHttpReferer());
		$this->layout->assign('requestPath',    PXRegistry::getRequest()->getPathAsString());
		$this->layout->assign('REGEX_MOD',      REGEX_MOD);
	}

	function html() {
		$html = PXRegistry::getLayout()->display();
		
		$response =& PXResponse::getInstance();
		$response->send($html);
	}

	function runModules() {
		PXProfiler::begin('RUN MODULES');

		$queue = PXRegistry::getApp()->bindingsQueue;
		$queue->getRequestBinding();
		foreach($queue as $bind) {
			if(!$bind->isBinding($this->content)) {
				continue;
			}

			$moduleDescription = $bind->getModuleDescription();
			$debugKey = 'MODULE: '.$moduleDescription->class;

			PXProfiler::begin($debugKey);
			$module = $moduleDescription->getModule();

			$module->tree       =& $this->content->tree;
			$module->objects    =& $this->content->objects;
			$module->subObjects =& $this->content->subObjects;

			$module->userIndex();
			PXProfiler::end();
		}

		PXProfiler::end();
	}
}
?>
