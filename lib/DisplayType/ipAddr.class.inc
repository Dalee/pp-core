<?php
/**
 * ip адрес
 *
 * storageType integer
 * displayType IPADDR
 */
class PXDisplayTypeIpaddr extends PXDisplayType {
	function buildInput($field, $object, $param=NULL) {
		$k = $field->name;
		$value = explode(".", long2ip(isset($object[$k]) ? $object[$k] : 0));

		return NLAbstractHTMLForm::BuildIPAddr($k, $value);
	}

	function buildCell($field, $object, $param=NULL) {
		return long2ip(isset($object[$field->name]) ? $object[$field->name] : 0);
	}

	function getFromRequest($field, $object, $param=NULL) {
		if ($param['request']->getHttpMethod() == 'POST') {
			return $param['request']->postData->_GetIPAddrVar($field->name);
		} else {
			return $param['request']->getData->_GetIPAddrVar($field->name);
		}
	}
}
?>