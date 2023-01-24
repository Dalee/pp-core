<?php
// abstract
class PXXSLT
{
    public $parameters = [];

    public function __construct(public $xsl)
    {
    }

    public function addParameter($key, $value)
    {
        $this->parameters[$key] = $value;
    }

    public function setXSL($xsl)
    {
        $this->xsl = $xsl;
    }

    // abstract
    public function transform($xml, $convertToKoi = true)
    {
        FatalError('It\s  abstract method');
    }
}
