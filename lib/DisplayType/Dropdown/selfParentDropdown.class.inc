<?php
/**
 * Выпадающий список с деревом объектов этого PXTypeDescription типа (как правило поле pid)
 *
 * storageType integet
 * displayType SELFPARENTDROPDOWN|width
 *             width не используется
 */
class PXDisplayTypeSelfparentdropdown extends PXDisplayTypeParentdropdown
{
    public function buildInput($field, $object, $param = NULL)
    {
        $value = $object[$field->name] ?? NULL;
        $selfTree = parent::getSelfParentTree($object, $param);
        $selfTree = $selfTree->getPlainTree(getFromArray($object, 'id'));
        if (isset($param['root_title'])) {
            $selfTree[NULL] = $param['root_title'];
        }

        return NLAbstractHTMLForm::BuildDropDown($field->name, $value, $selfTree);
    }
}

