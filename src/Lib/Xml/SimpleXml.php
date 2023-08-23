<?php

namespace PP\Lib\Xml;

/**
 * Class SimpleXml
 * @package PP\Lib\Xml
 */
class SimpleXml extends AbstractXml
{
    /**
     * SimpleXml constructor.
     * @param $xmlEntity
     */
    public function __construct($xmlEntity)
    {
        $this->xmlObject = $this->identEntity(
            $xmlEntity,
            'SimpleXMLElement',
            'simplexml_load_file',
            'simplexml_load_string'
        );
    }

    /**
     * {@inheritdoc}
     */
    public function xpath($query)
    {
        $nodesContainer = [];

        if (is_object($this->xmlObject)) {
            $nodes = $this->xmlObject->xpath($query);

            if ($nodes) {
                foreach ($nodes as $node) {
                    $nodesContainer[] = new SimpleXmlNode($node);
                }
            }
        }

        return $nodesContainer;
    }
}
