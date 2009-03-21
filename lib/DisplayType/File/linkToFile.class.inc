<?php
/**
 * Ссылка на файл, лежащий где то еще (например в /download/)
 * для выбора файла используется модуль PXModuleFile
 *
 * storageType serialized
 * displayType LINKTOFILE
 */
class PXDisplayTypeLinktofile extends PXDisplayTypeFile {
	function buildInput($field, $object, $param=NULL) {
		$k = $field->name;

		$value = (isset($object[$k]) && is_array($object[$k]) && isset($object[$k]['filename']) && !is_null($object[$k]['filename']))
			? $object[$k]
			: array('filename'=>null, 'dir'=>null, 'size'=>null);

		return NLAbstractHTMLForm::BuildLinkToFile($k, $value);
	}

	function getFromRequest($field, $object, $param=NULL) {
		return $param['request']->postData->_GetArrayVar($field->name);
	}

	function buildCell($field, $object, $param=NULL) {
		$value = $object[$field->name];
		if(!isset($value['filename'])) {
			return '';
		}

		$tmp = array(
			'filename' => $value['filename'],
			'fullpath' => $value['dir'].$value['filename'],
			'size'     => $value['size']
		);

		return $this->buildCellHtml($tmp);
	}
}
?>