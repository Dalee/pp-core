<?php
class PXEngineIndex extends PXEngine {
	function __construct() {     // éÎÉÃÉÁÌÉÚÁÃÉÑ ÏÓÎÏ×ÎÙÈ ÏÂßÅËÔÏ× ÐÒÉÌÏÖÅÎÉÑ
		PXProfiler::begin('TOTAL');

		parent::__construct();
		PXRegistry::setLayout(new PXUserHTMLLayout());
		$this->saveToRegistry();

		$this->content             = new StdClass; // FIXME
		$this->content->tree       = new PXTreeObjects();
		$this->content->objects    = new PXObjects();
		$this->content->subObjects = new PXSubObjects();

		$this->fillLayout();
	}

	function initModules() {
	}

	function fillLayout() {
		$layout = PXRegistry::getLayout();
		$layout->setApp(PXRegistry::getApp());

		// ðÅÒÅÄÁÞÁ ÄÁÎÎÙÈ × ÛÁÂÌÏÎÉÚÁÔÏÒ
		$layout->assign('app',            PXRegistry::getApp(),      true);
		$layout->assign('user',           PXRegistry::getUser(),     true);
		$layout->assign('request',        PXRegistry::getRequest(),  true);
		$layout->assign('response',       PXResponse::getInstance(), true);

		$layout->assignRequest(PXRegistry::getRequest());
		$layout->assign('tree',           $this->content->tree);
/*
		$layout->assign('references',     PXRegistry::getVar('references'));                 // delete
		$layout->assign('heap',           PXRegistry::getVar('heap'));                       // delete
*/
//		$layout->assign('currentSid',     PXRegistry::getVar('struct')->currentId);          // move to tree

		$layout->assign('objects',        $this->content->objects);
//		$layout->assign('currentCid',     PXRegistry::getVar('content')->currentId);         // move to objects
//		$layout->assign('currentCtype',   PXRegistry::getVar('content')->currentType);       // move to objects

		$layout->assign('subObjects',     $this->content->subObjects);
//		$layout->assign('currentSCid',    PXRegistry::getVar('subContent')->currentId);      // move to subObjects
//		$layout->assign('currentSCtype',  PXRegistry::getVar('subContent')->currentType);    // move to subObjects

//		$layout->assign('rootId',         PXRegistry::getVar('rootId'));                     // move to tree
//		$layout->assign('pathId',         PXRegistry::getVar('pathId'));                     // move to tree
//		$layout->assign('urlFile',        PXRegistry::getVar('urlFile'));                    // move to request

//		$layout->assign('error',          PXRegistry::getVar('error'));

		// Deprecated features
		$currentSid  = $this->content->tree->hasCurrent()       ? $this->content->tree->getCurrent()->id       : -1;
		if($this->content->objects->hasCurrent()){
			$objs = $this->content->objects->getCurrent();
			$currentCid  = $objs['id'];
		} else {
			$currentCid  = -1;
		}

		if($this->content->subObjects->hasCurrent()){
			$objs = $this->content->subObjects->getCurrent();
			$currentSCid  = $objs['id'];
		} else {
			$currentSCid  = -1;
		}

		$layout->assign('currentSid',  $currentSid);
		$layout->assign('currentCid',  $currentCid);
		$layout->assign('currentSCid', $currentSCid);

		$layout->assign('currentCtype', $this->content->objects->getCurrentType());

		$layout->assign('pathId', $this->content->tree->pathId);
		$layout->assign('rootId', $this->content->tree->rootId);
		$layout->assign('urlFile', PXRegistry::getRequest()->getFile());

		$layout->assign('requestHost',    PXRegistry::getRequest()->getHttpHost());
		$layout->assign('requestUri',     $_SERVER['REQUEST_URI']);
		$layout->assign('requestReferer', PXRegistry::getRequest()->getHttpReferer());
		$layout->assign('requestPath',    $_SERVER['SCRIPT_NAME']);
	}

	function html() {
		$html = PXRegistry::getLayout()->display();
		
		$response =& PXResponse::getInstance();
		$response->send($html);
	}

	function runModules() {
		PXProfiler::begin('RUN MODULES');

		foreach(PXRegistry::getApp()->bindingsQueue as $bind) {
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
