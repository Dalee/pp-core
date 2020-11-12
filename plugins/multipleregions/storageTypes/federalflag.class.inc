<?php
/**
 * Логическое поле, флаг, доступный для модификации только федеральным редакторам
 * Рекомендуется дополнительно защищать объекты от регионального клонирования с помощью deny_region_edit
 * В базе может храниться как BOOL
 *
 * displayType CHECKBOX
 * storageType federalflag
 */
class PXStorageTypeFederalFlag extends PXStorageTypeBoolean
{
    public function normalizeObjectAttribute($field, $object, &$param)
    {
        $regP = PXMultiRegions::getInstance();
        if ($regP->isRegionalObject($field->typeDescription) && $this->_fieldBeingModified($field, $object) && !$regP->isFederal()) {
            $object[$field->name] = $this->restoreValue; //WHADDADO?: maibe raise error is more preferred way instead of silently discard changes ?
        }

        return parent::normalizeObjectAttribute($field, $object, $param);
    }

    private function _fieldBeingModified($field, $object)
    {
        $modified = false;
        $objectInDb = [];

        if (empty($object['id'])) {
            $modified = !!$object[$field->name]; //FIXME: maybe check against default value ?
        } else {
            $db = PXRegistry::getDb();
            $objectInDb = $db->getObjectById($field->typeDescription, $object['id']);

            $modified = empty($objectInDb) || $objectInDb[$field->name] != $object[$field->name];
        }

        $this->restoreValue = $objectInDb ? $objectInDb[$field->name] : false; //FIXME: maybe check against default value ?

        return $modified;
    }
}
?>
