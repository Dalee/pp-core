<?php
class PXEngineIndex extends PXEngine {
	function __construct() {     // éÎÉÃÉÁÌÉÚÁÃÉÑ ÏÓÎÏ×ÎÙÈ ÏÂßÅËÔÏ× ÐÒÉÌÏÖÅÎÉÑ
		PXProfiler::begin('TOTAL');

		parent::__construct();

		PXRegistry::setLayout(new PXUserHTMLLayout());

		$this->content             = new StdClass; // FIXME

		$this->content->tree       = new PXTreeObjects();
		$this->content->objects    = new PXObjects();
		$this->content->subObjects = new PXSubObjects();
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

/*
		$layout->assign('references',     PXRegistry::getVar('references'));                 // delete
		$layout->assign('heap',           PXRegistry::getVar('heap'));                       // delete
*/
		$layout->assign('tree',           $this->content->tree);
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

			$module->tree       = $this->content->tree;
			$module->objects    = $this->content->objects;
			$module->subObjects = $this->content->subObjects;

			$module->userIndex();
			PXProfiler::end();
		}

		PXProfiler::end();
	}
}
?>