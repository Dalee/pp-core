<?php

class PXMultipleRegionsHelper
{
    public static function toArray($regString)
    {
        return self::uniq(array_filter(explode(",", strtr($regString, ["{" => "", "}" => ""])), "is_numeric"));
    }

    public static function toString($regArray)
    {
        return sprintf("{%s}", join(",", array_filter(self::uniq($regArray), "is_numeric")));
    }

    public static function uniq($regArray)
    {
        return array_flip(array_flip($regArray));
    }

    public static function simpleSetDbValues($key, $value, &$dbFields, &$dbValues)
    {
        $keys = array_flip($dbFields);

        if (!isset($keys[$key])) {
            $dbFields[] = $key;
            $dbValues[] = $value;
        } else {
            $dbValues[$keys[$key]] = $value;
        }
    }

    public static function createDirectoryTree($id = null, $fieldname = 'shortname', $delim = '/~', $tree = null)
    {
        $regions = PXMultiRegions::getInstance();

        if (!$tree) {
            $tree = $regions->getRegionsData(true);
        }

        if (!$id) {
            $id = $regions->getRid();
        }

        if (isset($tree->leafs[$id]) && isset($tree->leafs[$id]->content[$fieldname])) {
            $path = $tree->leafs[$id]->content[$fieldname];
        } else {
            $path = '';
        }

        if (isset($tree->leafs[$id]->parent) && $tree->leafs[$id]->parent) {
            return self::createDirectoryTree($tree->leafs[$id]->parent, $fieldname, "/~", $tree) . $delim . $path;
        }

        return $delim . $path;
    }

    public function createSingleDirectory($allRegionsData, $regionId, $fieldname = 'shortname', $delim = '/~')
    {
        return isset($allRegionsData[$regionId]) && isset($allRegionsData[$regionId][$fieldname]) ? $delim . $allRegionsData[$regionId][$fieldname] : '';
    }

    /*{{{{ for tree bubbling */
    public static function getVarRecursive($field, $rid = null, $data = null)
    {
        if (!$data) {
            $regions = PXMultiRegions::getInstance();
            $data = $regions->getRegionsData(true);

            if (!$rid) {
                $rid = $regions->getRid();
            }
        }

        if (!isset($data->leafs[$rid])) {
            return;
        }

        if (!(isset($data->leafs[$rid]->content[$field]) &&
            is_scalar($data->leafs[$rid]->content[$field]) ?
                mb_strlen($data->leafs[$rid]->content[$field]) :
                !empty($data->leafs[$rid]->content[$field])) &&
            ($parent = $data->leafs[$rid]->parent())) {

            return self::getVarRecursive($field, $parent, $data);
        }

        return $data->leafs[$rid]->content[$field];
    }

    public static function getTreeVar($field, $rid = null, $regions = null)
    {
        if (!$rid) {
            $rInst = PXMultiRegions::getInstance();
            $rid = $rInst->getRid();
        }

        if (empty($regions)) {
            $rInst = PXMultiRegions::getInstance();
            $regions = $rInst->getRegionsData(true);
        }

        $fields = [];
        if (!isset($regions->leafs[$rid])) {
            return [];
        }

        $fields[$rid] = $regions->leafs[$rid]->content[$field];
        if ($parent = $regions->leafs[$rid]->parent) {
            $fields = self::getTreeVar($field, $parent, $regions) + $fields;
        }

        return $fields;
    }

    /* }}}} */

    public static function switchRegion($host, $rid)
    {
        $regions = PXMultiRegions::getInstance();
        $regionsData = $regions->getRegionsData();

        if (!isset($regionsData[$rid])) {
            return;
        }

        $engPrfx = PXRegistry::getApp()->getProperty('ENGLISH_HOST_PREFIX', 'english');

        if (mb_strstr($host, $engPrfx)) {
            $host = substr_replace($host, $engPrfx . '.', '');
            $isEng = true;
        }

        $currentRegion = $regionsData[$rid];
        $urlPattrens = explode(",", $currentRegion['url']);
        $host = explode(".", $host);

        $reversedHost = array_reverse($host);
        $result = [];

        foreach ($urlPattrens as $k => $i) {
            $iArr = explode(".", $i);

            if (current($iArr) == $engPrfx) {
                unset($iArr[0]);
            }

            $h = join(".", $iArr);

            $reversedPattern = array_reverse($iArr);

            foreach ($reversedPattern as $n => $k) {
                if (isset($reversedHost[$n]) && ($k == $reversedHost[$n] || $k == '*')) {
                    $result[$h][] = ($k == '*') ? $reversedHost[$n] : $k;
                } elseif ($n >= count($reversedHost) - 1) {
                    $result[$h][] = $k;
                }
            }

            $result[$h] = array_values(array_flip(array_flip($result[$h])));
        }

        uasort($result, [__CLASS__, 'sortRegions']);
    }

    private static function sortRegions($a, $b)
    {
        $a = count($a);
        $b = count($b);

        if ($a == $b) {
            return 0;
        }

        return $a < $b ? 1 : -1;
    }
}

?>
