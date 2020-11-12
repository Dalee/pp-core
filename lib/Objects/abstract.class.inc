<?php

use PP\Lib\Objects\ContentObjectsInterface;

abstract class PXContentObjects extends ArrayObject implements ContentObjectsInterface
{
    protected $data, $links, $linkedObjs; //FIXME
    protected $currentId, $currentType;

    public function __construct()
    {
        $this->currentId = -1;
        $this->currentType = -1;

        $this->links = [];
        $this->linkedObjs = [];
    }

    public function hasType($type)
    {
        return array_key_exists($type, $this) && sizeof($this[$type]);
    }

    public function getCurrentType()
    {
        return $this->currentType;
    }

    public function getLinks()
    {
        if (!$this->hasCurrent()) {
            return;
        }

        $format = PXRegistry::getTypes($this->currentType);

        if (!isset($format->references)) {
            return;
        }

        $db = PXRegistry::getDb();
        foreach ($format->references as $ref) {
            if ($ref->load == 'manual') {
                continue;
            }

            $otherType = $ref->getOther($format->id);
            $otherFormat = PXRegistry::getTypes($otherType);

            $links = $db->getLinks($ref, $format->id, $this->currentId);

            $this->links[$ref->name] = $links;
            $this->linkedObjs[$otherType] = $db->getObjectsByIdArray($otherFormat, true, array_keys($links));
        }
    }
}
