<?
include_once 'objects.class.inc';

class PXEngineIndex extends PXEngine {
	function __construct() {     // éÎÉÃÉÁÌÉÚÁÃÉÑ ÏÓÎÏ×ÎÙÈ ÏÂßÅËÔÏ× ÐÒÉÌÏÖÅÎÉÑ
		parent::__construct();
		PXRegistry::setLayout(new PXUserHTMLLayout());

		PXRegistry::setVar('struct',     new PXStruct());
		PXRegistry::setVar('content',    new PXContent());
		PXRegistry::setVar('subContent', new PXSubContent());
	}

	function fillLayout() {
		$layout = PXRegistry::getLayout();
		$layout->setApp(PXRegistry::getApp());

		// ðÅÒÅÄÁÞÁ ÄÁÎÎÙÈ × ÛÁÂÌÏÎÉÚÁÔÏÒ
		$layout->assign('app',            PXRegistry::getApp(),     true);
		$layout->assign('user',           PXRegistry::getUser(),    true);
		$layout->assign('request',        PXRegistry::getRequest(), true);

		$layout->assignRequest(PXRegistry::getRequest());

		$container = array(
		);

		$layout->assignArray($container, true);

		$layout->assign('tree',           PXRegistry::getVar('tree'));
		$layout->assign('requestHost',    PXRegistry::getRequest()->getHttpHost());
		$layout->assign('requestReferer', PXRegistry::getRequest()->getHttpReferer());

		$layout->assign('requestUri',     $_SERVER['REQUEST_URI']);
		$layout->assign('requestPath',    $_SERVER['SCRIPT_NAME']);
	}

	function sendHeaders() {
		if(headers_sent()) {
			return;
		}

		if ($this->container['error'] == 404) {
			if (function_exists('log404')) {
				log404(PXRegistry::getRequest());
			}

			/*
			header("HTTP/1.1 404");

			we can't send empty header.
			for more details see http://forums.iis.net/t/1146547.aspx
			*/
		}

		if (function_exists('sendheaders')) {
			sendheaders();
		}
	}

	function html() {
		PXRegistry::getLayout()->display();
	}

	function profilerStart() {
		NLProfiler('TOTAL', 'BEGIN');
	}

	function profilerStop() {
		NLProfiler('TOTAL', 'END');
		PXRegistry::getLayout()->assign('NLProfiler', NLProfiler(NULL, 'ALL'));
	}

	function runModules() {
		NLProfiler('RUN MODULES', 'BEGIN');
		$this->modules->activateModules('userindex', PXRegistry::getApp(), PXRegistry::getDb(), PXRegistry::getRequest(), PXRegistry::getUser(), PXRegistry::getLayout(), $this->container);
		NLProfiler('RUN MODULES', 'END');
	}
}
?>