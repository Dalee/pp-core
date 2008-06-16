<?php
class PXEngineIndex extends PXEngine {
	function __construct() {     // éÎÉÃÉÁÌÉÚÁÃÉÑ ÏÓÎÏ×ÎÙÈ ÏÂßÅËÔÏ× ÐÒÉÌÏÖÅÎÉÑ
		NLProfiler('TOTAL', 'BEGIN');

		parent::__construct();
		PXRegistry::setLayout(new PXUserHTMLLayout());

		PXRegistry::setVar('struct',     new PXStruct());
		PXRegistry::setVar('content',    new PXContent());
		PXRegistry::setVar('subContent', new PXSubContent());
	}

	function initModules() {
		$this->modules = new PXModuleLoader();
	}

	function fillLayout() {
		$layout = PXRegistry::getLayout();
		$layout->setApp(PXRegistry::getApp());

		// ðÅÒÅÄÁÞÁ ÄÁÎÎÙÈ × ÛÁÂÌÏÎÉÚÁÔÏÒ
		$layout->assign('app',            PXRegistry::getApp(),     true);
		$layout->assign('user',           PXRegistry::getUser(),    true);
		$layout->assign('request',        PXRegistry::getRequest(), true);

		$layout->assignRequest(PXRegistry::getRequest());

		$layout->assign('struct',         PXRegistry::getVar('struct'));
		$layout->assign('content',        PXRegistry::getVar('struct'));
		$layout->assign('subContent',     PXRegistry::getVar('struct'));
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

			if(IIS) {
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
		NLProfiler('TOTAL', 'END');
		PXRegistry::getLayout()->assign('NLProfiler', NLProfiler(NULL, 'ALL'));
		PXRegistry::getLayout()->display();
	}

	function runModules() {
		NLProfiler('RUN MODULES', 'BEGIN');

		foreach(PXRegistry::getApp()->bindingsQueue as $bind) {
			if(!$bind->isBinding()) {
				continue;
			}

			$moduleDescription = $bind->getModuleDescription();

			$debugKey = 'MODULE: '.$moduleDescription->class;

			NLProfiler($debugKey, 'BEGIN');
			$moduleDescription->getModule()->userIndex();
			NLProfiler($debugKey, 'END');
		}

		NLProfiler('RUN MODULES', 'END');
	}
}
?>