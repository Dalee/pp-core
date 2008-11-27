<?php
class PXEngineIndex extends PXEngine {
	function __construct() {     // éÎÉÃÉÁÌÉÚÁÃÉÑ ÏÓÎÏ×ÎÙÈ ÏÂßÅËÔÏ× ÐÒÉÌÏÖÅÎÉÑ
		PXProfiler::begin('TOTAL');

		parent::__construct();
		PXRegistry::setLayout(new PXUserHTMLLayout());

		PXRegistry::setVar('struct',     new PXStruct());
		PXRegistry::setVar('content',    new PXContent());
		PXRegistry::setVar('subContent', new PXSubContent());
	}

	function initModules() {
	}

	function fillLayout() {
		$layout = PXRegistry::getLayout();
		$layout->setApp(PXRegistry::getApp());

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
	}

	function sendHeaders() {
		if(headers_sent()) {
			return;
		}

		$error = PXRegistry::getVar('error');

		if ($error == 404) {
			if (function_exists('log404')) {
				log404(PXRegistry::getRequest());
			}

			if(IS_WIN) {
				/*
				we can't send empty header.
				for more details see http://forums.iis.net/t/1146547.aspx
				*/
				return;
			}

			header("HTTP/1.1 404");

		}

		if (function_exists('sendheaders')) {
			sendheaders();
		}
	}

	function html() {
		PXProfiler::end();
		PXRegistry::getLayout()->assign('PXProfilerResult', PXProfiler::finalize());
		PXRegistry::getLayout()->display();
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