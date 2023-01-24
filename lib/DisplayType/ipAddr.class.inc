<?php
/**
 * ip адрес
 *
 * storageType integer
 * displayType IPADDR
 */
class PXDisplayTypeIpaddr extends PXDisplayType
{
    public function buildInput($field, $object, $param = NULL)
    {
        $k = $field->name;
        $value = explode(".", long2ip($object[$k] ?? 0));

        return NLAbstractHTMLForm::BuildIPAddr($k, $value);
    }

    public function buildCell($field, $object, $param = NULL)
    {
        return long2ip($object[$field->name] ?? 0);
    }

    public function getFromRequest($field, $object, $param = NULL)
    {
        if ($param['request']->getHttpMethod() == 'POST') {
            return $param['request']->postData->_GetIPAddrVar($field->name);
        } else {
            return $param['request']->getData->_GetIPAddrVar($field->name);
        }
    }
}
?>
