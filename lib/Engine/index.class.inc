<?
class PXEngineIndex extends PXEngineInterface {
	function __construct() {     // éÎÉÃÉÁÌÉÚÁÃÉÑ ÏÓÎÏ×ÎÙÈ ÏÂßÅËÔÏ× ÐÒÉÌÏÖÅÎÉÑ
		parent::__construct();
		PXEngineObjects::setLayout(new PXUserHTMLLayout());
	}

	function fillLayout() {
		$layout = PXEngineObjects::getLayout();
		$layout->setApp(PXEngineObjects::getApp());

		// ðÅÒÅÄÁÞÁ ÄÁÎÎÙÈ × ÛÁÂÌÏÎÉÚÁÔÏÒ
		$layout->assign('app',            PXEngineObjects::getApp(),     true);
		$layout->assign('user',           PXEngineObjects::getUser(),    true);
		$layout->assign('request',        PXEngineObjects::getRequest(), true);

		$layout->assignRequest(PXEngineObjects::getRequest());

		$layout->assignArray($this->container, true);

		$layout->assign('requestHost',    PXEngineObjects::getRequest()->getHttpHost());
		$layout->assign('requestReferer', PXEngineObjects::getRequest()->getHttpReferer());

		$layout->assign('requestUri',     $_SERVER['REQUEST_URI']);
		$layout->assign('requestPath',    $_SERVER['SCRIPT_NAME']);
	}

	function sendHeaders() {
		if(headers_sent()) {
			return;
		}

		if ($this->container['error'] == 404) {
			if (function_exists('log404')) {
				log404(PXEngineObjects::getRequest());
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
		PXEngineObjects::getLayout()->display();
	}

	function profilerStart() {
		NLProfiler('TOTAL', 'BEGIN');
	}

	function profilerStop() {
		NLProfiler('TOTAL', 'END');
		PXEngineObjects::getLayout()->assign('NLProfiler', NLProfiler(NULL, 'ALL'));
	}

	function runModules() {
		NLProfiler('RUN MODULES', 'BEGIN');
		$this->modules->activateModules('userindex', PXEngineObjects::getApp(), PXEngineObjects::getDb(), PXEngineObjects::getRequest(), PXEngineObjects::getUser(), PXEngineObjects::getLayout(), $this->container);
		NLProfiler('RUN MODULES', 'END');
	}
}
?>