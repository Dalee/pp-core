<?php
/**
 * Файл
 *
 * storageType file
 * displayType FILE
 */
class PXDisplayTypeFile extends PXDisplayType
{
    public function buildInput($field, $object, $param = NULL)
    {
        $k = $field->name;
        $value = (isset($object[$k]) && is_array($object[$k]) && isset($object[$k]['filename']) && !is_null($object[$k]['filename']))
            ? $object[$k]
            : ['filename' => null, 'fullpath' => null, 'size' => null, 'type' => null];

        return NLAbstractHTMLForm::BuildFile($k, $value);
    }

    public function getFromRequest($field, $object, $param = NULL)
    {
        return $param['request']->postData->_GetFileVar($field->name);
    }

    public function buildCell($field, $object, $param = NULL)
    {
        return $this->buildCellHtml($object[$field->name]);
    }

    public function buildCellHtml(?array $fileArray)
    {
        if (!isset($fileArray['fullpath'])) {
            return '';
        }

        return '<a href="' . $fileArray['fullpath'] . '" class="file ' . preg_replace('/^.+\./' . REGEX_MOD, '', (string) $fileArray['filename']) . '">' . $fileArray['filename'] . '</a> ' . number_format($fileArray['size'] / 1024, 0, ',', ' ') . 'Кб</br>';
    }
}
