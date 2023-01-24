<?php
/**
 * Текстовое поле
 *
 * storageType string, integer или float
 * displayType TEXT|width|height|cell_maxlength|wrap
 *             width не используется
 *             если height = 0, то строится <input type="text">
 *             если height > 0, то строится <textarea> и height это ее высота в пикселях
 *             cell_maxlength определяет максимальное число символов, выводимых в ячейке
 *             wrap определяет тип переноса в textarea - physical, virtual, hard, soft (по умолчанию), off
 */
class PXDisplayTypeText extends PXDisplayType
{
    public function buildInput($field, $object, $param = NULL)
    {
        $value = $object[$field->name] ?? NULL;
        $height = isset($field->displayTypeArgs[1]) ? (int)$field->displayTypeArgs[1] : 0;
        $maxlength = !empty($field->displayTypeArgs[2]) ? (int)$field->displayTypeArgs[2] : NULL;
        $wrap = !empty($field->displayTypeArgs[3]) ? trim((string) $field->displayTypeArgs[3]) : "soft";

        return NLAbstractHTMLForm::BuildText($field->name, $value, $height, $maxlength, $wrap);
    }

    public function buildCell($field, $object, $param = NULL)
    {
        $trunc = isset($field->displayTypeArgs[2]) && (int)$field->displayTypeArgs[2] > 0 ? (int)$field->displayTypeArgs[2] : 50;
        $value = $object[$field->name] ?? '';
        $value = $this->getSubDirectoryIfExists($field, $object, $value);

        if (mb_strlen((string) $value) > $trunc) {
            $value = htmlspecialchars(mb_substr((string) $value, 0, $trunc), ENT_COMPAT | ENT_HTML401, DEFAULT_CHARSET) . '&hellip;';
        } else {
            $value = htmlspecialchars((string) $value, ENT_COMPAT | ENT_HTML401, DEFAULT_CHARSET);
        }

        return $value;
    }

    public function buildTreeHover($field, $object, $param = null)
    {
        return $this->buildCell($field, $object, $param);
    }
}
