<?php
/**
 * Список файлов
 *
 * storageType filesarray
 * displayType FILESARRAY
 */
class PXDisplayTypeFilesarray extends PXDisplayTypeImagesarray {
	function buildArrayItem($name, $value) {
		if(!strlen($value['entry'])) {
			$value = array(
				'filename' => null,
				'size'     => null,
				'fullpath' => null,
				'type'     => null,
				'entry'    => ''
			);
		}

		return NLAbstractHTMLForm::BuildMultimedia($name, $value, 'file', $value['entry']);
	}

	function buildCell($field, $object, $param=NULL) {
		$html = '';

		foreach($object[$field->name] as $item) {
			$html .= PXDisplayTypeFile::buildCellHtml($item);
		}

		return $html;
	}
}
?>