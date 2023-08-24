<?php

namespace PP\Lib\Xml;

class Xml
{
    public const NONE = 0;
    public const ELEMENT = 1;
    public const ATTRIBUTE = 2;
    public const DOC = 9;

    /** @var SimpleXml */
    public $xml;

    /**
     * Xml constructor.
     * @param $xmlEntity
     */
    public function __construct($xmlEntity)
    {

        $this->xml = match (true) {
            extension_loaded('simplexml') => new SimpleXml($xmlEntity),
            default => (object)['xmlObject' => false],
        };
    }

    /**
    * @param $fileName
    */
    public static function load($fileName): bool|\PP\Lib\Xml\SimpleXml
    {
        $instance = new Xml($fileName);
        return $instance->xml->xmlObject ? $instance->xml : false;
    }

    /**
    * @param $xmlDataInString
    */
    public static function loadString($xmlDataInString): bool|object
    {
        return Xml::load($xmlDataInString);
    }

    /**
     * @param $name
     * @param $value
     * @return object
     */
    public static function attributePrototype($name, $value)
    {
        return (object)['name' => $name, 'value' => $value];
    }
}
