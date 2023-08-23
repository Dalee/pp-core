<?php

namespace PP\Lib\Objects;

/**
 * Interface ContentObjectsInterface
 * @package PP\Lib\Objects
 */
interface ContentObjectsInterface
{
    public function hasCurrent();

    public function getCurrent();

    public function getAllowedChilds();

    /**
     * @param string $type
     * @return bool
     */
    public function hasType($type);

    public function getCurrentType();

    public function getLinks();
}
