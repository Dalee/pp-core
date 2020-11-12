<?php
/**
 * Строка
 * В базе может храниться как VARCHAR(x), так и TEXT
 *
 * displayType HIDDEN, TEXT, RICHEDIT, DROPDOWN, COLOR, STATIC
 * storageType string
 */
class PXStorageTypeString extends PXStorageType
{
    public function objectsSortOrder($field, $object, &$param = NULL)
    { /* E_STRICT */
        return $this->objectsSortOrderString($field, $object, $param);
    }

    public function getFromRequest($field, $object, &$param)
    {
        return (string)$object[$field->name];
    }

    public function normalizeObjectAttribute($field, $object, &$param)
    {
        return $this->normalizeObjectAttributeString($field, $object, $param);
    }

    public function searchLikeQuery($field, $object, &$param)
    {
        return $param['formatId'] . '.' . $field->name . PXRegistry::getDb()->LIKE($param['word'], P_LEFT | P_RIGHT) . 'OR ';
    }

    public function filterable()
    {
        return true;
    }
}
?>
