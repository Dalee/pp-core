<?php

class PXDeferredStatusHelper
{

    protected static $activeStates = [2, 3];

    protected static $sysdeferredlist_values = [
        1 => ['id' => 1, 'tag' => 'off', 'title' => 'Отключено'],
        2 => ['id' => 2, 'tag' => 'prepared', 'title' => 'К публикации'],
        3 => ['id' => 3, 'tag' => 'published', 'title' => 'Опубликовано'],
        9 => ['id' => 9, 'tag' => 'done', 'title' => 'Снято'],
    ];

    protected static $sysdeferredlist_tagmap;

    private static function prepareTagIdMap()
    {
        if (empty(self::$sysdeferredlist_tagmap)) {
            self::$sysdeferredlist_tagmap = array_flat(self::$sysdeferredlist_values, 'tag', 'id');
        }
    }

    public static function init()
    {
        self::prepareTagIdMap();
    }

    public static function normalizeState($state, $default = null)
    {
        if (!empty($state) && isset(self::$sysdeferredlist_values[$state])) {
            return $state;
        }

        if (!empty($state) && isset(self::$sysdeferredlist_tagmap[$state])) {
            return self::$sysdeferredlist_tagmap[$state];
        }

        if (is_null($default)) {
            return null;
        }

        return self::$sysdeferredlist_tagmap[$default];
    }

    public static function stateTag($state)
    {
        if (isset(self::$sysdeferredlist_tagmap[$state])) {
            $state = self::$sysdeferredlist_tagmap[$state];
        }
        return self::$sysdeferredlist_values[$state]['tag'];
    }

    public static function values()
    {
        return self::$sysdeferredlist_values;
    }

    public static function states($ids)
    {
        return array_intersect_key(self::$sysdeferredlist_values, array_flip($ids));
    }

    public static function statesIds($a)
    {
        $in = is_array($a) ? $a : func_get_args();

        $out = [];
        foreach ($in as $tag) {
            $out[] = self::normalizeState($tag);
        }

        return array_filter($out);
    }

    public static function activeStatesIds()
    {
        return self::$activeStates;
    }

    public static function applyNextState(&$v)
    {

        // $state = self::stateTag($v['sys_deferred_state']);
        $since = strtotime($v['sys_deferred_since']);
        $until = strtotime($v['sys_deferred_until']);
        $now = time();

        switch (true) {
            case $since === false && $until === false: // Даты не проставлены, ставим состояние "Отключено"
                $state = 'off';
                break;
            case self::stateTag($v['sys_deferred_state']) == 'off': // Даты проставлены, но состояние "Отключено" - ничего не делаем
                return;
            case $now < $since: // $since date will be tomorrow
                $state = 'prepared';
                $v['status'] = false;
                break;
            case $since <= $now && !$until: // time to publish forever
                $state = 'done';
                $v['status'] = true;
                break;
            case !$since && $now <= $until: // without publishing date
                $v['sys_deferred_since'] = date('Y-m-d H:i:s', $now); // set current time as publishing date
            case $since <= $now && $now <= $until: // now between since and until, usual case
                $state = 'published';
                $v['status'] = true;
                break;
            case $until < $now: // stop publishing
                $state = 'done';
                $v['status'] = false;
                break;
            default:
                $state = 'off';
                $v['status'] = false;
        }

        $v['sys_deferred_state'] = self::normalizeState($state);

    }

}
