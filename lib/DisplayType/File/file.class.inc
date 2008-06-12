<?php
/**
 * æÁÊÌ
 *
 * storageType file
 * displayType FILE
 */
class PXDisplayTypeFile extends PXDisplayType {
	static function buildInput($field, $object, $param=NULL) {
		$k = $field->name;
		$value = (isset($object[$k]) && is_array($object[$k]) && isset($object[$k]['filename']) && !is_null($object[$k]['filename']))
			? $object[$k]
			: array('filename'=>null, 'fullpath'=>null, 'size'=>null, 'type'=>null);

		return NLAbstractHTMLForm::BuildFile($k, $value);
	}

	static function getFromRequest($field, $object, $param=NULL) {
		return $param['request']->postData->_GetFileVar($field->name);
	}

	static function buildCell($field, $object, $param=NULL) {
		return PXDisplayTypeFile::buildCellHtml($object[$field->name]);
	}

	static function buildCellHtml($fileArray) {
		if(!isset($fileArray['fullpath'])) {
			return '';
		}

		return '<a href="'.$fileArray['fullpath'].'" class="file '.preg_replace('/^.+\./', '', $fileArray['filename']).'">'.$fileArray['filename'].'</a> '.number_format($fileArray['size']/1024, 0, ',', ' ').'ëÂ</br>';
	}
}
?>
