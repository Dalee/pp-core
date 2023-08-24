<?php

namespace PP\Lib\Config;

use PXTypeDescription;

interface ApplicationInterface
{
    /**
     * @param string $formatName
     * @return array
     */
    public function initContentObject(string $formatName): array;

    /**
     * @param string $typeId
     * @param PXTypeDescription $type
     * @return void
     */
    public function setDataType(string $typeId, PXTypeDescription $type): void;

    /**
     * @param string $typeId
     * @return PXTypeDescription|null
     */
    public function getDataType(string $typeId): ?PXTypeDescription;
}
