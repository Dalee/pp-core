<?php
class PXRequestUserJson extends PXRequestUser {
	var $ajax;

	function __construct() {
		include_once 'Subsys_JsHttpRequest/lib/Subsys/JsHttpRequest/Php.php';
		$this->ajax =  new Subsys_JsHttpRequest_Php(DEFAULT_CHARSET);
		$this->ajax->RESULT = array();
ppl(serialize($this->ajax));
		if (!isset($_GET['charcheck'])) {
			$_GET['charcheck'] = myconv('k', DEFAULT_CHARSET, 'ÊÃÕËÅÎ');
		}

		//$this->PXRequest(); # So strange...
		parent::__construct();

		if(isset($_REQUEST['get-js-code'])) {
			$response =& PXResponse::getInstance();
			ob_end_clean();
			$response->send(myconv('w', 'k', $this->ajax->getJsCode()));
			exit;
		}
	}

	function appendResult($varName, $value) {
		$this->ajax->RESULT[$varName] = $value;
	}

	function getArea() {
		return isset($_GET['area']) ? $_GET['area'] : NULL;
	}

	function setResult($result) {
		$this->ajax->RESULT = $result;
	}
}
?>