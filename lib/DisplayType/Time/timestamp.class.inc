<?php
/**
 * Дата и время (с календарем)
 *
 * storageType timestamp
 * displayType TIMESTAMP
 */
class PXDisplayTypeTimestamp extends PXDisplayType
{
    public function buildInput($field, $object, $param = NULL)
    {
        $k = $field->name;
        $value = (isset($object[$k]) && $object[$k] !== 'null') ? $object[$k] : ($field->defaultValue == 'null' ? NULL : date('j-n-Y G:i:s'));

        return NLAbstractHTMLForm::BuildDateTime($k, $value, true, true, true);
    }

    public function buildCell($field, $object, $param = NULL)
    {
        return $object[$field->name];
    }

    public function getFromRequest($field, $object, $param = NULL)
    {
        return $param['request']->postData->_GetTimeStampVar($field->name);
    }
}
?>
