<?php
class PXEngineIndex extends PXEngine {
	function __construct() {     // éÎÉÃÉÁÌÉÚÁÃÉÑ ÏÓÎÏ×ÎÙÈ ÏÂßÅËÔÏ× ÐÒÉÌÏÖÅÎÉÑ
		PXProfiler::begin('TOTAL');

		parent::__construct();
		PXRegistry::setLayout(new PXUserHTMLLayout());
/*
		PXRegistry::setVar('struct',     new PXStruct());
		PXRegistry::setVar('content',    new PXContent());
		PXRegistry::setVar('subContent', new PXSubContent());
*/
	}

	function initModules() {
	}

	function fillLayout() {
		$layout = PXRegistry::getLayout();
		$layout->setApp(PXRegistry::getApp());

/*
		// ðÅÒÅÄÁÞÁ ÄÁÎÎÙÈ × ÛÁÂÌÏÎÉÚÁÔÏÒ
		$layout->assign('app',            PXRegistry::getApp(),     true);
		$layout->assign('user',           PXRegistry::getUser(),    true);
		$layout->assign('request',        PXRegistry::getRequest(), true);

		$layout->assignRequest(PXRegistry::getRequest());

		$layout->assign('references',     PXRegistry::getVar('references'));
		$layout->assign('heap',           PXRegistry::getVar('heap'));

		$layout->assign('tree',           PXRegistry::getVar('struct')->tree);
		$layout->assign('currentSid',     PXRegistry::getVar('struct')->currentId);

		$layout->assign('objects',        PXRegistry::getVar('content')->data);
		$layout->assign('currentCid',     PXRegistry::getVar('content')->currentId);
		$layout->assign('currentCtype',   PXRegistry::getVar('content')->currentType);

		$layout->assign('subObjects',     PXRegistry::getVar('subContent')->data);
		$layout->assign('currentSCid',    PXRegistry::getVar('subContent')->currentId);
		$layout->assign('currentSCtype',  PXRegistry::getVar('subContent')->currentType);

		$layout->assign('rootId',         PXRegistry::getVar('rootId'));
		$layout->assign('pathId',         PXRegistry::getVar('pathId'));
		$layout->assign('urlFile',        PXRegistry::getVar('urlFile'));

		$layout->assign('error',          PXRegistry::getVar('error'));
*/
	}

	function sendHeaders() {
		$error = 0;

/*
		$error = PXRegistry::getVar('error');
*/

		$response =& PXResponse::getInstance();

		if ($error == 404) {
			$response->notFound();

			if (function_exists('log404')) {
				log404(PXRegistry::getRequest());
			}
		}
	}

	function html() {
		$html = PXRegistry::getLayout()->display();

		$response =& PXResponse::getInstance();
		$response->send($html);
	}

	function runModules() {
		PXProfiler::begin('RUN MODULES');

		foreach(PXRegistry::getApp()->bindingsQueue as $bind) {
			if(!$bind->isBinding()) {
				continue;
			}

			$moduleDescription = $bind->getModuleDescription();

			$debugKey = 'MODULE: '.$moduleDescription->class;

			PXProfiler::begin($debugKey);
			$moduleDescription->getModule()->userIndex();
			PXProfiler::end($debugKey);
		}

		PXProfiler::end();
	}
}
?>