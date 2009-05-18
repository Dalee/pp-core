<?php
class PXSablotron extends PXXSLT {
	var $arguments;

	function convertToKoi8r($result) {
		static $rus_chars = array(
			'%c3%a1' => '%C0', // А
			'%c3%a2' => '%C1', // Б
			'%c3%b7' => '%C2', // В
			'%c3%a7' => '%C3', // Г
			'%c3%a4' => '%C4', // Д
			'%c3%a5' => '%C5', // Е
			'%c3%b6' => '%C6', // Ж
			'%c3%ba' => '%C7', // З
			'%c3%a9' => '%C8', // И
			'%c3%aa' => '%C9', // Й
			'%c3%ab' => '%СA', // К
			'%c3%ac' => '%CB', // Л
			'%c3%ad' => '%CC', // М
			'%c3%ae' => '%CD', // Н
			'%c3%af' => '%CE', // О
			'%c3%b0' => '%CF', // П
			'%c3%b2' => '%D0', // Р
			'%c3%b3' => '%D1', // С
			'%c3%b4' => '%D2', // Т
			'%c3%b5' => '%D3', // У
			'%c3%a6' => '%D4', // Ф
			'%c3%a8' => '%D5', // Х
			'%c3%a3' => '%D6', // Ц
			'%c3%be' => '%D7', // Ч
			'%c3%bb' => '%D8', // Ш
			'%c3%bd' => '%D9', // Щ
			'%c3%bf' => '%DA', // Ъ
			'%c3%b9' => '%DB', // Ы
			'%c3%b8' => '%DC', // Ь
			'%c3%bc' => '%DD', // Э
			'%c3%a0' => '%DE', // Ю
			'%c3%b1' => '%DF', // Я
			'%c3%81' => '%E0', // а
			'%c3%82' => '%E1', // б
			'%c3%97' => '%E2', // в
			'%c3%87' => '%E3', // г
			'%c3%84' => '%E4', // д
			'%c3%85' => '%E5', // е
			'%c3%96' => '%E6', // ж
			'%c3%9a' => '%E7', // з
			'%c3%89' => '%E8', // и
			'%c3%8a' => '%E9', // й
			'%c3%8b' => '%EA', // к
			'%c3%8c' => '%EB', // л
			'%c3%8d' => '%EC', // м
			'%c3%8e' => '%ED', // н
			'%c3%8f' => '%EE', // о
			'%c3%90' => '%EF', // п
			'%c3%92' => '%F0', // р
			'%c3%93' => '%F1', // с
			'%c3%94' => '%F2', // т
			'%c3%95' => '%F3', // у
			'%c3%86' => '%F4', // ф
			'%c3%88' => '%F5', // х
			'%c3%83' => '%F6', // ц
			'%c3%9e' => '%F7', // ч
			'%c3%9b' => '%F8', // ш
			'%c3%9d' => '%F9', // щ
			'%c3%9f' => '%FA', // ъ
			'%c3%99' => '%FB', // ы
			'%c3%98' => '%FC', // ь
			'%c3%9c' => '%FD', // э
			'%c3%80' => '%FE', // ю
			'%c3%91' => '%FF'  // я
		);

		$result = utf8_decode($result);

		$result = strtr($result, array_flip(get_html_translation_table(HTML_ENTITIES)));

		$result = str_replace('<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">', '', $result);
		$result = str_replace(' xmlns:xsl="http://www.w3.org/1999/XSL/Transform"', '', $result);


		return strtr($result, $rus_chars);
	}


	function XSLTError($parser, $num, $level, $messages) {
		$fileName = ($messages['URI'] == 'arg:/_xsl') ? 'ROOT' : $messages['URI'];

		$message  = '<p>Error in file <em>'.$fileName.'</em>, line <em>'.$messages['line'].'</em>:</p>';
		$message .= '<p>'.$messages['msg'].'</p>';

		FatalError($message);
	}

	function transform($xmlString, $convertToKoi=true) {
		foreach ($this->parameters as $k=>$v) {
			$this->parameters[$k] = utf8_encode($v);
		}

		$xh = xslt_create();
		xslt_set_error_handler($xh, array(&$this, "XSLTError"));

		$this->arguments['/_xml'] = str_replace('<?xml version="1.0" encoding="koi8-r"?>', '<?xml version="1.0" encoding="iso-8859-1"?>', $xmlString);

		$html = xslt_process($xh, 'arg:/_xml', $this->xsl, NULL, $this->arguments, $this->parameters);

		xslt_free($xh);

		if($convertToKoi) {
			$html = $this->convertToKoi8r($html);
		}

		return $html;
	}

}
?>