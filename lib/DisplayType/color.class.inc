<?php
/**
 * Выбора цвета
 *
 * storageType string / VARCHAR(6)
 * displayType COLOR
 */
class PXDisplayTypeColor extends PXDisplayType
{
    public function buildInput($field, $object, $param = NULL)
    {
        $value = isset($object[$field->name]) ? $object[$field->name] : NULL;
        return NLAbstractHTMLForm::BuildColor($field->name, $value);
    }

    public function buildCell($field, $object, $param = NULL)
    {
        $value = $object[$field->name];
        return ctype_alnum($value) && (mb_strlen($value) == 3 || mb_strlen($value) == 6) ? '<div class="rgbcolor"><div style="background-color: #' . $value . ';" title="#' . $value . '"></div></div>' : '';
    }
}

