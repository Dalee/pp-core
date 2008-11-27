<?php
// temporary alias
class PXRequest extends PXRequestUser {
}

class PXRequestUser extends PXRequestBase {
	// ������� ���������� "������" ������������� ����������
	function getArea($default=NULL) {
		$area = $this->_getGetPostVar('area') ? $this->_getGetPostVar('area') : $this->getGetVar('area');
		return ($area == NULL) ? $default : $area;
	}

	// ������� ����������� ������ "��������" � ������������ ���������� ��������� ������
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

	// ���������� ������� ������ ������ "struct"
	function getSid() {
		return $this->_getGetPostVar('sid');
	}

	// ���������� ������� ������ ������ "content"
	function getCid() {
		return $this->_getGetPostVar('cid');
	}

	// ����������� id �������������� �������
	function getId() {
		return $this->_getGetPostVar('id');
	}

	// ����������� format ��������������/������������ �������
	function getFormat() {
		return $this->_getGetPostVar('format');
	}

	// ����������� parent ��������������/������������ �������
	function getParent() {
		return $this->_getGetPostVar('parent');
	}

	// ����������� �������� ������������ ��� �������������/����������� ��������
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
?>