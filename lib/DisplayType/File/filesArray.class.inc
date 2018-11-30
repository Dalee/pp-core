<?php
/**
 * Список файлов
 *
 * storageType filesarray
 * displayType FILESARRAY
 */
class PXDisplayTypeFilesarray extends PXDisplayTypeImagesarray {
	function buildArrayItem($name, $value) {
		if(!mb_strlen($value['entry'])) {
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

		$dtFile = new PXDisplayTypeFile($this->display, $this->name);

		foreach($object[$field->name] as $item) {
			$html .= $dtFile->buildCellHtml($item);
		}

		return $html;
	}
}
?>
