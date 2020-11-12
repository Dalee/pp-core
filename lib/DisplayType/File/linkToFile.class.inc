<?php
/**
 * Ссылка на файл, лежащий где то еще (например в /download/)
 * для выбора файла используется модуль PP\Module\FileModule
 *
 * storageType serialized
 * displayType LINKTOFILE
 */
class PXDisplayTypeLinktofile extends PXDisplayTypeFile
{
	public function buildInput($field, $object, $param = NULL)
	{
		$k = $field->name;

		$value = (isset($object[$k]) && is_array($object[$k]) && isset($object[$k]['filename'])
				? $object[$k]
				: []
			) + ['filename' => null, 'dir' => null, 'size' => null];

		return NLAbstractHTMLForm::BuildLinkToFile($k, $value);
	}

	public function getFromRequest($field, $object, $param = NULL)
	{
		return $param['request']->postData->_GetArrayVar($field->name);
	}

	public function buildCell($field, $object, $param = NULL)
	{
		$value = $object[$field->name];
		if (!isset($value['filename'])) {
			return '';
		}

		$tmp = [
			'filename' => $value['filename'],
			'fullpath' => $value['dir'] . $value['filename'],
			'size' => $value['size'],
		];

		return $this->buildCellHtml($tmp);
	}
}
?>
