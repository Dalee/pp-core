<?php
/**
 * æÁÊÌ
 *
 * storageType file
 * displayType FILE
 */
class PXDisplayTypeFile extends PXDisplayType {
	function buildInput($field, $object, $param=NULL) {
		$k = $field->name;
		$value = (isset($object[$k]) && is_array($object[$k]) && isset($object[$k]['filename']) && !is_null($object[$k]['filename']))
			? $object[$k]
			: array('filename'=>null, 'fullpath'=>null, 'size'=>null, 'type'=>null);

		return NLAbstractHTMLForm::BuildFile($k, $value);
	}

	function getFromRequest($field, $object, $param=NULL) {
		return $param['request']->postData->_GetFileVar($field->name);
	}

	function buildCell($field, $object, $param=NULL) {
		return $this->buildCellHtml($object[$field->name]);
	}

	function buildCellHtml() {
		list($fileArray) = func_get_args(); // PHP54 args contract violation
		if(!isset($fileArray['fullpath'])) {
			return '';
		}

		return '<a href="'.$fileArray['fullpath'].'" class="file '.preg_replace('/^.+\./'.REGEX_MOD, '', $fileArray['filename']).'">'.$fileArray['filename'].'</a> '.number_format($fileArray['size']/1024, 0, ',', ' ').'ëÂ</br>';
	}
}
?>
