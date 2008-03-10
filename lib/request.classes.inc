<?php
class PXNullRequest {
	function getVar() {
		return NULL;
	}
}


class PXBaseRequest extends NLRequest {
	function PXBaseRequest() {
		$this->NLRequest();
	}

	// Функция возвращает "раздел" редакторского интерфейса
	function getArea($default=NULL) {
		$area = $this->_getGetPostVar('area') ? $this->_getGetPostVar('area') : $this->getGetVar('area');
		return ($area == NULL) ? $default : $area;
	}
}

class PXRequest extends PXBaseRequest {
	function PXRequest() {
		$this->PXBaseRequest();
	}

	// Функция возвраящает массив "открытых" в редакторском интерфейсе элементов дерева
	function getLeafStatus() {
		$leafStatus = $this->cookieData->_getVar('leafStatus');
		$openLeaf   = array();
		if ($leafStatus) {
			foreach ($leafStatus as $l=>$s) {
				if ($s == 'open') {
					$openLeaf[] = substr($l, 6);
				}
			}
		}
		return $openLeaf;
	}

	// Возвращает текущий объект уровня "struct"
	function getSid() {
		return $this->_getGetPostVar('sid');
	}

	// Возвращает текущий объект уровня "content"
	function getCid() {
		return $this->_getGetPostVar('cid');
	}

	// Возвращаяет id редактируемого объекта
	function getId() {
		return $this->_getGetPostVar('id');
	}

	// Возвращаяет format редактируемого/добавляемого объекта
	function getFormat() {
		return $this->_getGetPostVar('format');
	}

	// Возвращаяет parent редактируемого/добавляемого объекта
	function getParent() {
		return $this->_getGetPostVar('parent');
	}

	// Возвращаяет действие производимое над редактируемым/добавляемым объектом
	function getAction() {
		return $this->_getGetPostVar('action');
	}

	// POST
	function getLinks($reference) {
		return $this->postData->_getArrayVar($reference->name, false);
	}

	// POST
	function getContentObject($format) {
		$object = array();
		foreach ($format->fields as $k=>$v) {
			$object[$k] = PXDisplayType::callMethod('getFromRequest', $v, $object, array('request' => $this));
		}

		foreach ($format->fields as $k=>$v) {
			if (!empty($v->handler)) {
				$file = strtolower($v->handler).".inputhandler.inc";

				if (file_exists(BASEPATH.'/local/lib/handlers/'.$file)) {
					include_once BASEPATH.'/local/lib/handlers/'.$file;

				} elseif (file_exists(BASEPATH.'/libpp/lib/handlers/'.$file)) {
					include_once BASEPATH.'/libpp/lib/handlers/'.$file;
				}

				$function = 'PXInputHandler'.$v->handler;

				if (function_exists($function)) {
					$object[$k] = $function($v, $object[$k]);
				}
			}
		}

		foreach ($format->fields as $k=>$v) {
			$object[$k] = PXStorageType::callMethod('getFromRequest', $v, $object, array('request' => $this));
		}
		return $object;
	}

	function getObjectSysVars($format, $neededVars = array()) {
		$object['id'] = $this->postData->_getNumericVar('id');

		if ($this->postData->_IsSet(OBJ_FIELD_CHILDREN) || in_array(OBJ_FIELD_CHILDREN, $neededVars)) {
			$object[OBJ_FIELD_CHILDREN] = $this->postData->_GetArrayVar(OBJ_FIELD_CHILDREN);
		}

		if ($this->postData->_IsSet('sys_accessmod')) {
			$object['sys_accessmod'] = $this->postData->_getNumericVar('sys_accessmod');
		}

		if ($this->postData->_IsSet('sys_accessput')) {
			$object['sys_accessput'] = $this->postData->_getNumericVar('sys_accessput');
		}

		return $object;
	}

	// POST
	function getAfterActionDeal() {
		return $this->postData->_IsSetVar('close') ? 'close' : 'back';
	}

	/* From PXUserRequest */
	function getHostAndDir() {
		$a = $this->getPath();
		array_unshift($a, $this->getHttpHost());
		if (sizeof($a) > 1 && strstr($a[sizeof($a)-1], ".")) {
			array_pop($a);
		}
		return $a;
	}

	function getFile() {
		$a = $this->getPath();
		if (count($a)) {
			if (strstr($a[sizeof($a)-1], ".")) {
				return $a[sizeof($a)-1];
			}
		}

		return 'index.html';
	}

	function getPart() {
		return $this->_getGetPostVar('part');
	}

}

class PXUserJsonRequest extends PXRequest {
	var $ajax;

	function PXUserJsonRequest() {
		include_once 'Subsys_JsHttpRequest/lib/Subsys/JsHttpRequest/Php.php';
		$this->ajax =  new Subsys_JsHttpRequest_Php(DEFAULT_CHARSET);
		$this->ajax->RESULT = array();

		if (!isset($_GET['charcheck'])) {
			$_GET['charcheck'] = myconv('k', DEFAULT_CHARSET, 'йцукен');
		}

		$this->PXRequest();

		if(isset($_REQUEST['get-js-code'])) {
			header("X-Accel-Expires: 3600");

			ob_end_clean();
			echo myconv('w', 'k', $this->ajax->getJsCode());
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
