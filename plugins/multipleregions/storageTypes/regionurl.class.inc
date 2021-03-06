<?php
/**
 * Строка URL регионов
 * В базе может храниться как VARCHAR(x), так и TEXT
 *
 * displayType URLTEXT
 * storageType regionurl
 */
class PXStorageTypeRegionUrl extends PXStorageTypeString
{
    public function getFromRequest($field, $object, &$param)
    {
        $k = $field->name;
        if (isset($object[$k]) && count($object[$k])) {
            $urls = [];

            foreach ($object[$k] as $url) {
                $urls[] = $url[0];
            }

            return implode(',', $urls);
        }
    }
}
?>
