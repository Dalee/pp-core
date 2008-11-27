<?php
/**
 * ������ �� ����, ������� ��� �� ��� (�������� � /download/)
 * ��� ������ ����� ������������ ������ PXModuleFile
 *
 * storageType serialized
 * displayType LINKTOFILE
 */
class PXDisplayTypeLinktofile extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$k = $field->name;

		$value = (isset($object[$k]) && is_array($object[$k]) && isset($object[$k]['filename']) && !is_null($object[$k]['filename']))
			? $object[$k]
			: array('filename'=>null, 'dir'=>null, 'size'=>null);

		return NLAbstractHTMLForm::BuildLinkToFile($k, $value);
	}

	static function getFromRequest($field, $object, $param=NULL) {
		return $param['request']->postData->_GetArrayVar($field->name);
	}

	static function buildCell($field, $object, $param=NULL) {
		$value = $object[$field->name];
		if(!isset($value['filename'])) {
			return '';
		}

		$tmp = array(
			'filename' => $value['filename'],
			'fullpath' => $value['dir'].$value['filename'],
			'size'     => $value['size']
		);

		return PXDisplayTypeFile::buildCellHtml($tmp);
	}
}
?>