<?php

class PXLibXSLT extends PXXSLT
{
    public function convertToKoi8r($result)
    {
        static $rus_chars = [
            '%C3%A1' => '%C0', // А
            '%C3%A2' => '%C1', // Б
            '%C3%b7' => '%C2', // В
            '%C3%A7' => '%C3', // Г
            '%C3%A4' => '%C4', // Д
            '%C3%A5' => '%C5', // Е
            '%C3%B6' => '%C6', // Ж
            '%C3%BA' => '%C7', // З
            '%C3%A9' => '%C8', // И
            '%C3%AA' => '%C9', // Й
            '%C3%AB' => '%СA', // К
            '%C3%AC' => '%CB', // Л
            '%C3%AD' => '%CC', // М
            '%C3%AE' => '%CD', // Н
            '%C3%AF' => '%CE', // О
            '%C3%B0' => '%CF', // П
            '%C3%B2' => '%D0', // Р
            '%C3%B3' => '%D1', // С
            '%C3%B4' => '%D2', // Т
            '%C3%B5' => '%D3', // У
            '%C3%A6' => '%D4', // Ф
            '%C3%A8' => '%D5', // Х
            '%C3%A3' => '%D6', // Ц
            '%C3%BE' => '%D7', // Ч
            '%C3%BB' => '%D8', // Ш
            '%C3%BD' => '%D9', // Щ
            '%C3%BF' => '%DA', // Ъ
            '%C3%B9' => '%DB', // Ы
            '%C3%B8' => '%DC', // Ь
            '%C3%BC' => '%DD', // Э
            '%C3%A0' => '%DE', // Ю
            '%C3%B1' => '%DF', // Я
            '%C3%81' => '%E0', // а
            '%C3%82' => '%E1', // б
            '%C3%97' => '%E2', // в
            '%C3%87' => '%E3', // г
            '%C3%84' => '%E4', // д
            '%C3%85' => '%E5', // е
            '%C3%96' => '%E6', // ж
            '%C3%9A' => '%E7', // з
            '%C3%89' => '%E8', // и
            '%C3%8A' => '%E9', // й
            '%C3%8B' => '%EA', // к
            '%C3%8C' => '%EB', // л
            '%C3%8D' => '%EC', // м
            '%C3%8E' => '%ED', // н
            '%C3%8F' => '%EE', // о
            '%C3%90' => '%EF', // п
            '%C3%92' => '%F0', // р
            '%C3%93' => '%F1', // с
            '%C3%94' => '%F2', // т
            '%C3%95' => '%F3', // у
            '%C3%86' => '%F4', // ф
            '%C3%88' => '%F5', // х
            '%C3%83' => '%F6', // ц
            '%C3%9E' => '%F7', // ч
            '%C3%9B' => '%F8', // ш
            '%C3%9D' => '%F9', // щ
            '%C3%9F' => '%FA', // ъ
            '%C3%99' => '%FB', // ы
            '%C3%98' => '%FC', // ь
            '%C3%9C' => '%FD', // э
            '%C3%80' => '%FE', // ю
            '%C3%91' => '%FF'  // я
        ];

        $result = mb_convert_encoding((string) $result, 'ISO-8859-1');
        $result = strtr($result, array_flip(get_html_translation_table(HTML_ENTITIES)));

        return strtr($result, $rus_chars);
    }

    public function _dom($xmlString)
    {
        $xml = new DomDocument();
        $xml->loadXML($xmlString);

        $xslf = new DomDocument();
        $xslf->load($this->xsl);

        $xsl = new xsltprocessor();
        $xsl->importStyleSheet($xslf);

        foreach ($this->parameters as $k => $v) {
            $xsl->setParameter('', $k, mb_convert_encoding((string) $v, 'UTF-8', 'ISO-8859-1'));
        }

        return $xsl->transformToXML($xml);
    }

    public function transform($xmlString, $convertToKoi = true)
    {
        $html = $this->_dom($xmlString);

        if ($convertToKoi) {
            $html = $this->convertToKoi8r($html);
        }

        return $html;
    }
}
