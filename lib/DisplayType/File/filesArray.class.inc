<?php
/**
 * Список файлов
 *
 * storageType filesarray
 * displayType FILESARRAY
 */
class PXDisplayTypeFilesarray extends PXDisplayTypeImagesarray
{
	public function buildArrayItem($name, $value)
	{
		if (!mb_strlen((string) $value['entry'])) {
			$value = [
				'filename' => null,
				'size' => null,
				'fullpath' => null,
				'type' => null,
				'entry' => '',
			];
		}

		return NLAbstractHTMLForm::BuildMultimedia($name, $value, 'file', $value['entry']);
	}

	public function buildCell($field, $object, $param = NULL)
	{
		if (empty($object[$field->name]) || !is_array($object[$field->name])) {
			return '';
		}

		$html = '';

		$dtFile = new PXDisplayTypeFile($this->display, $this->name);

		foreach ($object[$field->name] as $item) {
			$html .= $dtFile->buildCellHtml($item);
		}

		return $html;
	}
}
