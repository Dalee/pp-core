<?php
/**
 * Checkbox
 *
 * storageType boolean
 * displayType CHECKBOX
 */
class PXDisplayTypeCheckbox extends PXDisplayType
{
    public function buildInput($field, $object, $param = NULL)
    {
        $value = isset($object[$field->name]) && $object[$field->name];
        return NLAbstractHTMLForm::BuildInputCheckBox($field->name, $value, null, isset($param['id']) ? $param['id'] : null);
    }

    public function buildCell($field, $object, $param = NULL)
    {
        $value = $object[$field->name];
        return $value ? '<div class="truecheck" title="да"></div>' : '';
    }

    public function getFromRequest($field, $object, $param = NULL)
    {
        return $param['request']->postData->_GetCheckBoxVar($field->name);
    }
}
?>
