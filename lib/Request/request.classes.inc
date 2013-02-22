<?php
class PXRequestAbstract extends PXRequest {}
class PXRequestBase     extends PXRequestAbstract {}
class PXRequestUser     extends PXRequestBase {}

class PXNullRequest extends PXRequestAbstract {
	public function __construct() {
	}

	public function GetVar($varName, $default = NULL) {
		return NULL;
	}
	
	public function GetUploadFile($varName, $isArray = false){
		return array();
	}
	
}

?>