<?php
/**
 * Project:     Dalee PHP-classes library
 * File:        common.functions.inc
 *
 * @copyright   Dalee, 2003-2010
 * @author      Michael Beloshapka <mb@dalee.ru>
 * @package     ProximaPortal
 * @subpackage  Common
 */

function _PrepareWMLText($string) {
	$string = preg_replace("/\\$\s*(\d+([,|.]\d+)?)/si", "\\1 Õ.Å.", $string);

	if(preg_match_all("/<a\s.*?(href=\"(.+?)\").*?>(.+?)<\/a>/si", $string, $urls)) {
		$string = preg_replace("/<a\s.*?href=\"(.+?)\".*?>(.+?)<\/a>/si", "[a \\1]\\2[/a]", $string);
	}

	$string = strip_tags($string);
	$string = nl2br($string);

	$string = preg_replace("/\[a (.*?)\](.*?)\[\/a\]/si", "<a href=\"\\1\">\\2</a>", $string);

	return $string;
}

function wapUtf8Cyr($str) {
	static $trans = array(
		'Á' => '&#x0430;', 'Â' => '&#x0431;', '×' => '&#x0432;', 'Ç' => '&#x0433;', 'Ä' => '&#x0434;',
		'Å' => '&#x0435;', 'Ö' => '&#x0436;', 'Ú' => '&#x0437;', 'É' => '&#x0438;', 'Ê' => '&#x0439;',
		'Ë' => '&#x043A;', 'Ì' => '&#x043B;', 'Í' => '&#x043C;', 'Î' => '&#x043D;', 'Ï' => '&#x043E;',
		'Ð' => '&#x043F;', 'Ò' => '&#x0440;', 'Ó' => '&#x0441;', 'Ô' => '&#x0442;', 'Õ' => '&#x0443;',
		'Æ' => '&#x0444;', 'È' => '&#x0445;', 'Ã' => '&#x0446;', 'Þ' => '&#x0447;', 'Û' => '&#x0448;',
		'Ý' => '&#x0449;', 'ß' => '&#x044A;', 'Ù' => '&#x044B;', 'Ø' => '&#x044C;', 'Ü' => '&#x044D;',
		'À' => '&#x044E;', 'Ñ' => '&#x044F;',

		'á' => '&#x0410;', 'â' => '&#x0411;', '÷' => '&#x0412;', 'ç' => '&#x0413;', 'ä' => '&#x0414;',
		'å' => '&#x0415;', 'ö' => '&#x0416;', 'ú' => '&#x0417;', 'é' => '&#x0418;', 'ê' => '&#x0419;',
		'ë' => '&#x041A;', 'ì' => '&#x041B;', 'í' => '&#x041C;', 'î' => '&#x041D;', 'ï' => '&#x041E;',
		'ð' => '&#x041F;', 'ò' => '&#x0420;', 'ó' => '&#x0421;', 'ô' => '&#x0422;', 'õ' => '&#x0423;',
		'æ' => '&#x0424;', 'è' => '&#x0425;', 'ã' => '&#x0426;', 'þ' => '&#x0427;', 'û' => '&#x0428;',
		'ý' => '&#x0429;', 'ÿ' => '&#x042A;', 'ù' => '&#x042B;', 'ø' => '&#x042C;', 'ü' => '&#x042D;',
		'à' => '&#x042E;', 'ñ' => '&#x042F;',

		'&nbsp;'   => ' ',
		'&laquo;'  => '&#x00AB;',
		'&raquo;'  => '&#x00BB;',
		'&bdquo;'  => '&#x0093;',
		'&ldquo;'  => '&#x0084;',
//		'&rdquo;'  => '&#x0093;',
		'&rdquo;'  => '\'',
		'&#8212;'  => '-',
		'&mdash;'  => '-',
		'&ndash;'  => '-',
		'&#150;'   => '-',
		'&#8211;'  => '&#x2013;',
		'&hellip;' => '...',
		'&#8217;'  => '&#x2019;',
		'&#8470;'  => '&#x2116;',

//		'&' => '&#x0026;', '$' => '&#xFE69; &#xFF04;', '#' => '&#x0023;',
//		'&' => '&#x0026;', '$' => '&#xFF04;', '#' => '&#x0023;',
		'&' => '&#x0026;', '#' => '&#x0023;',

		'&#x2190;' => '&#x003C;', '&#x2191;' => '&#x003C;', '&#x2192;' => '&#x003E;', '&#x2193;' => '&#x003E;',
	);

	return strtr($str, $trans);
}

function utfDecode($string) {
	return utfEncode($string, 'decode');
}

function utfEncode($string, $direction = 'encode') {
	static $tran = array(
		"%A8"=>"%D0%81", "%B8"=>"%D1%91", "%C0"=>"%D0%90", "%C1"=>"%D0%91", "%C2"=>"%D0%92", "%C3"=>"%D0%93",
		"%C4"=>"%D0%94", "%C5"=>"%D0%95", "%C6"=>"%D0%96", "%C7"=>"%D0%97", "%C8"=>"%D0%98", "%C9"=>"%D0%99",
		"%CA"=>"%D0%9A", "%CB"=>"%D0%9B", "%CC"=>"%D0%9C", "%CD"=>"%D0%9D", "%CE"=>"%D0%9E", "%CF"=>"%D0%9F",
		"%D0"=>"%D0%A0", "%D1"=>"%D0%A1", "%D2"=>"%D0%A2", "%D3"=>"%D0%A3", "%D4"=>"%D0%A4", "%D5"=>"%D0%A5",
		"%D6"=>"%D0%A6", "%D7"=>"%D0%A7", "%D8"=>"%D0%A8", "%D9"=>"%D0%A9", "%DA"=>"%D0%AA", "%DB"=>"%D0%AB",
		"%DC"=>"%D0%AC", "%DD"=>"%D0%AD", "%DE"=>"%D0%AE", "%DF"=>"%D0%AF", "%E0"=>"%D0%B0", "%E1"=>"%D0%B1",
		"%E2"=>"%D0%B2", "%E3"=>"%D0%B3", "%E4"=>"%D0%B4", "%E5"=>"%D0%B5", "%E6"=>"%D0%B6", "%E7"=>"%D0%B7",
		"%E8"=>"%D0%B8", "%E9"=>"%D0%B9", "%EA"=>"%D0%BA", "%EB"=>"%D0%BB", "%EC"=>"%D0%BC", "%ED"=>"%D0%BD",
		"%EE"=>"%D0%BE", "%EF"=>"%D0%BF", "%F0"=>"%D1%80", "%F1"=>"%D1%81", "%F2"=>"%D1%82", "%F3"=>"%D1%83",
		"%F4"=>"%D1%84", "%F5"=>"%D1%85", "%F6"=>"%D1%86", "%F7"=>"%D1%87", "%F8"=>"%D1%88", "%F9"=>"%D1%89",
		"%FA"=>"%D1%8A", "%FB"=>"%D1%8B", "%FC"=>"%D1%8C", "%FD"=>"%D1%8D", "%FE"=>"%D1%8E", "%FF"=>"%D1%8F",
	);

	static $typo = array(
		"%C2%A7"=>"&#167;",
		"%C2%A9"=>"&#169;",
		"%C2%AB"=>"&#171;",
		"%C2%AE"=>"&#174;",
		"%C2%B0"=>"&#176;",
		"%C2%B1"=>"&#177;",
		"%C2%BB"=>"&#187;",
		"%E2%80%93"=>"&#150;",
		"%E2%80%94"=>"&#151;",
		"%E2%80%9C"=>"&#147;",
		"%E2%80%9D"=>"&#148;",
		"%E2%80%9E"=>"&#132;",
		"%E2%80%A6"=>"&#133;",
		"%E2%84%96"=>"&#8470;",
		"%E2%84%A2"=>"&#153;",
		"%C2%A4"=>"&curren;",
		"%C2%B6"=>"&para;",
		"%C2%B7"=>"&middot;",
		"%E2%80%98"=>"&#145;",
		"%E2%80%99"=>"&#146;",
		"%E2%80%A2"=>"&#149;",
	);

	static $rtypo = array(
		"%84"=>"%E2%80%9E",
		"%85"=>"%E2%80%A6",
		"%91"=>"%E2%80%98",
		"%92"=>"%E2%80%99",
		"%93"=>"%E2%80%9C",
		"%94"=>"%E2%80%9D",
		"%95"=>"%E2%80%A2",
		"%96"=>"%E2%80%93",
		"%97"=>"%E2%80%94",
		"%99"=>"%E2%84%A2",
		"%A7"=>"%C2%A7",
		"%A9"=>"%C2%A9",
		"%AB"=>"%C2%AB",
		"%AE"=>"%C2%AE",
		"%B0"=>"%C2%B0",
		"%B1"=>"%C2%B1",
		"%BB"=>"%C2%BB",
	);

	if ($direction == "encode") {
		$tran = array_merge($tran,$rtypo);
		$string = strtr(urlencode($string),$tran);
	} else {
		$rtran = array_flip($tran);
		$rtran = array_merge($rtran,$typo);
		$string = strtr(urlencode($string),$rtran);
	}

	return urldecode($string);
}

function _StrToLower($text) {
	$text  = strtolower($text);
	$trans = array('À' => 'à', 'Á' => 'á', 'Â' => 'â', 'Ã' => 'ã', 'Ä' => 'ä', 'Å' => 'å', '£' => '³', 'Æ' => 'æ', 'Ç' => 'ç', 'È' => 'è', 'É' => 'é', 'Ê' => 'ê', 'Ë' => 'ë', 'Ì' => 'ì', 'Í' => 'í', 'Î' => 'î', 'Ï' => 'ï', 'Ð' => 'ð', 'Ñ' => 'ñ', 'Ò' => 'ò', 'Ó' => 'ó', 'Ô' => 'ô', 'Õ' => 'õ', 'Ö' => 'ö', '×' => '÷', 'Ø' => 'ø', 'Ù' => 'ù', 'Ú' => 'ú', 'Û' => 'û', 'Ü' => 'ü', 'Ý' => 'ý', 'Þ' => 'þ', 'ß' => 'ÿ');
	$trans = array_flip($trans);
	$text  = strtr($text, $trans);
	return $text;
}

function _StrToUpper($text) {
	$text  = strtoupper($text);
	$trans = array('À' => 'à', 'Á' => 'á', 'Â' => 'â', 'Ã' => 'ã', 'Ä' => 'ä', 'Å' => 'å', '³' => '£', 'Æ' => 'æ', 'Ç' => 'ç', 'È' => 'è', 'É' => 'é', 'Ê' => 'ê', 'Ë' => 'ë', 'Ì' => 'ì', 'Í' => 'í', 'Î' => 'î', 'Ï' => 'ï', 'Ð' => 'ð', 'Ñ' => 'ñ', 'Ò' => 'ò', 'Ó' => 'ó', 'Ô' => 'ô', 'Õ' => 'õ', 'Ö' => 'ö', '×' => '÷', 'Ø' => 'ø', 'Ù' => 'ù', 'Ú' => 'ú', 'Û' => 'û', 'Ü' => 'ü', 'Ý' => 'ý', 'Þ' => 'þ', 'ß' => 'ÿ');
//	$trans = array_flip($trans);
	$text  = strtr($text, $trans);
	return $text;
}

function _Translit($text) {
	$trans = array(
		'á' => 'A',  'â' => 'B',  '÷' => 'V',   'ç' => 'G', 'ä' => 'D',  'å' => 'E',
		'³' => 'E',  'ö' => 'G',  'ú' => 'ZH',  'é' => 'I', 'ê' => 'JO', 'ë' => 'K',
		'ì' => 'L',  'í' => 'M',  'î' => 'N',   'ï' => 'O', 'ð' => 'P',  'ò' => 'R',
		'ó' => 'S',  'ô' => 'T',  'õ' => 'U',   'æ' => 'F', 'è' => 'H',  'ã' => 'TS',
		'þ' => 'CH', 'û' => 'SH', 'ý' => 'SCH', 'ÿ' => '',  'ù' => 'Y',  'ø' => '',
		'ü' => 'E',  'à' => 'JU', 'ñ' => 'JA',
		'Á' => 'a',  'Â' => 'b',  '×' => 'v',   'Ç' => 'g', 'Ä' => 'd',  'Å' => 'e',
		'£' => 'jo', 'Ö' => 'zh', 'Ú' => 'z',   'É' => 'i', 'Ê' => 'jo', 'Ë' => 'k',
		'Ì' => 'l',  'Í' => 'm',  'Î' => 'n',   'Ï' => 'o', 'Ð' => 'p',  'Ò' => 'r',
		'Ó' => 's',  'Ô' => 't',  'Õ' => 'y',   'Æ' => 'f', 'È' => 'h',  'Ã' => 'ts',
		'Þ' => 'ch', 'Û' => 'sh', 'Ý' => 'sch', 'ß' => '',  'Ù' => 'y',  'Ø' => '',
		'Ü' => 'e',  'À' => 'yu', 'Ñ' => 'ya'
	);
	$text = strtr($text, $trans);
	return $text;
}

function _TranslitFilename($text) {
	$trans = array(
		'á' => 'a',     'Á' => 'a',     'â' => 'b',     'Â' => 'b',
		'÷' => 'v',     '×' => 'v',     'ç' => 'g',     'Ç' => 'g',
		'ä' => 'd',     'Ä' => 'd',     'å' => 'e',     'Å' => 'e',     '³' => 'e',     '£' => 'e',
		'ö' => 'zh',    'Ö' => 'xh',    'ú' => 'z',     'Ú' => 'z',
		'é' => 'i',     'É' => 'i',     'ê' => 'j',     'Ê' => 'j',
		'ë' => 'k',     'Ë' => 'k',     'ì' => 'l',     'Ì' => 'l',
		'í' => 'm',     'Í' => 'm',     'î' => 'n',     'Î' => 'n',
		'ï' => 'o',     'Ï' => 'o',     'ð' => 'p',     'Ð' => 'p',
		'ò' => 'r',     'Ò' => 'r',     'ó' => 's',     'Ó' => 's',
		'ô' => 't',     'Ô' => 't',     'õ' => 'u',     'Õ' => 'u',
		'æ' => 'f',     'Æ' => 'f',     'è' => 'h',     'È' => 'h',
		'ã' => 'c',     'Ã' => 'c',     'þ' => 'ch',    'Þ' => 'ch',
		'û' => 'sh',    'Û' => 'sh',    'ý' => 'sch',   'Ý' => 'sch',
		'ÿ' => '',      'ß' => '',      'ù' => 'i',     'Ù' => 'i',
		'ø' => '',      'Ø' => '',      'ü' => 'e',     'Ü' => 'e',
		'à' => 'u',     'À' => 'u',     'ñ' => 'ya',    'Ñ' => 'ya',

		' ' => '_',     ',' => '_',     '?' => '_',
		'&' => '_',     '#' => '_',     '$' => '_',     '%' => '_',
		'@' => '_',     '!' => '_',     '~' => '_',     '`' => '_',
		':' => '_',     '"' => '_',     '\'' => '_',    '\\' => '_',
		'/' => '_',     '<' => '_',     '>' => '_',     '^' => '_',
		'*' => '_',     '(' => '_',     ')' => '_',     '+' => '_',
		'|' => '_',     ';' => '_',     '=' => '_',     '[' => '_',
		']' => '_',     '{' => '_',     '}' => '_',

		"\n" => ''
	);

	$text = strtr(trim($text), $trans);
	if(strpos($text, '.')) {
		$text = explode('.', $text);
		$text[0] = substr($text[0], 0, 32);
		$text = implode('.', $text);
	} else {
		 $text = substr($text, 0, 32);
	}

	return $text;
}

function _TranslitUnique($text) {
	$trans = array(
		'á' => 'A',   'â' => 'B',  '÷' => 'V',   'ç' => 'G', 'ä' => 'D',  'å' => 'E',
		'³' => 'EO',  'ö' => 'ZH', 'ú' => 'Z',   'é' => 'I', 'ê' => 'JO', 'ë' => 'K',
		'ì' => 'L',   'í' => 'M',  'î' => 'N',   'ï' => 'O', 'ð' => 'P',  'ò' => 'R',
		'ó' => 'S',   'ô' => 'T',  'õ' => 'U',   'æ' => 'F', 'è' => 'H',  'ã' => 'TS',
		'þ' => 'CH',  'û' => 'SH', 'ý' => 'SCH', 'ÿ' => '',  'ù' => 'Y',  'ø' => '',
		'ü' => 'JE',  'à' => 'JU', 'ñ' => 'JA',
		'Á' => 'a',   'Â' => 'b',  '×' => 'v',   'Ç' => 'g', 'Ä' => 'd',  'Å' => 'e',
		'£' => 'jo',  'Ö' => 'zh', 'Ú' => 'z',   'É' => 'i', 'Ê' => 'jo', 'Ë' => 'k',
		'Ì' => 'l',   'Í' => 'm',  'Î' => 'n',   'Ï' => 'o', 'Ð' => 'p',  'Ò' => 'r',
		'Ó' => 's',   'Ô' => 't',  'Õ' => 'y',   'Æ' => 'f', 'È' => 'h',  'Ã' => 'ts',
		'Þ' => 'ch',  'Û' => 'sh', 'Ý' => 'sch', 'ß' => '',  'Ù' => 'y',  'Ø' => '',
		'Ü' => 'je',  'À' => 'yu', 'Ñ' => 'ya'
	);
	$text = strtr($text, $trans);
	return $text;
}

function _UnTranslitUnique($text) {
	$trans = array_flip(array(
		'á' => 'A',   'â' => 'B',  '÷' => 'V',   'ç' => 'G', 'ä' => 'D',  'å' => 'E',
		'³' => 'EO',  'ö' => 'ZH',  'ú' => 'Z',  'é' => 'I', 'ê' => 'JO', 'ë' => 'K',
		'ì' => 'L',   'í' => 'M',  'î' => 'N',   'ï' => 'O', 'ð' => 'P',  'ò' => 'R',
		'ó' => 'S',   'ô' => 'T',  'õ' => 'U',   'æ' => 'F', 'è' => 'H',  'ã' => 'TS',
		'þ' => 'CH',  'û' => 'SH', 'ý' => 'SCH', 'ù' => 'Y',
		'ü' => 'JE',  'à' => 'JU', 'ñ' => 'JA',
		'Á' => 'a',   'Â' => 'b',  '×' => 'v',   'Ç' => 'g', 'Ä' => 'd',  'Å' => 'e',
		'£' => 'jo',  'Ö' => 'zh', 'Ú' => 'z',   'É' => 'i', 'Ê' => 'jo', 'Ë' => 'k',
		'Ì' => 'l',   'Í' => 'm',  'Î' => 'n',   'Ï' => 'o', 'Ð' => 'p',  'Ò' => 'r',
		'Ó' => 's',   'Ô' => 't',  'Õ' => 'y',   'Æ' => 'f', 'È' => 'h',  'Ã' => 'ts',
		'Þ' => 'ch',  'Û' => 'sh', 'Ý' => 'sch', 'Ù' => 'y',
		'Ü' => 'je',  'À' => 'yu', 'Ñ' => 'ya'
	));
	$text = strtr($text, $trans);
	return $text;
}

function _TranslitModern($text) {
	$trans = array(
		'á' => 'A',  'â' => 'B',  '÷' => 'V',   'ç' => 'G', 'ä' => 'D',  'å' => 'E',
		'³' => 'E',  'ö' => 'Zh', 'ú' => 'Z',   'é' => 'I', 'ê' => 'Y',  'ë' => 'K',
		'ì' => 'L',  'í' => 'M',  'î' => 'N',   'ï' => 'O', 'ð' => 'P',  'ò' => 'R',
		'ó' => 'S',  'ô' => 'T',  'õ' => 'U',   'æ' => 'F', 'è' => 'H',  'ã' => 'Ts',
		'þ' => 'Ch', 'û' => 'Sh', 'ý' => 'Sch', 'ÿ' => '',  'ù' => 'Y',  'ø' => '',
		'ü' => 'E',  'à' => 'Yu', 'ñ' => 'Ya',

		'Á' => 'a',  'Â' => 'b',  '×' => 'v',   'Ç' => 'g', 'Ä' => 'd',  'Å' => 'e',
		'£' => 'e',  'Ö' => 'zh', 'Ú' => 'z',   'É' => 'i', 'Ê' => 'y',  'Ë' => 'k',
		'Ì' => 'l',  'Í' => 'm',  'Î' => 'n',   'Ï' => 'o', 'Ð' => 'p',  'Ò' => 'r',
		'Ó' => 's',  'Ô' => 't',  'Õ' => 'u',   'Æ' => 'f', 'È' => 'h',  'Ã' => 'ts',
		'Þ' => 'ch', 'Û' => 'sh', 'Ý' => 'sch', 'ß' => '',  'Ù' => 'y',  'Ø' => '',
		'Ü' => 'e',  'À' => 'yu', 'Ñ' => 'ya',  "\xbf" => 'N'
	);
	$text = strtr($text, $trans);
	return $text;
}

function NumericEndingsRussian($count, $oneItem, $severalItiems, $manyItems) {
	$lastLetter       = substr($count, -1);
	$secondLastLetter = strlen($count) > 1 ? substr($count, -2, 1) : '0';

	if($secondLastLetter != 1) {
		if($lastLetter == '1') {
			return $oneItem;
		}

		if(in_array($lastLetter, array('2', '3', '4'))) {
			return $severalItiems;
		}
	}

	return $manyItems;
}

function htmlspecialcharsArray($array) {
	if (!is_array($array)) return array();
	foreach($array as $k=>$v) {
		if (is_array($v)) {
			$array[$k] = htmlspecialcharsArray($v);
		} else {
			$array[$k] = htmlspecialchars($v);
		}
	}
	return $array;
}

function DbQuoteString($s) {
	return "'".addslashes($s)."'";
}

function Strip1251($s) {
	static $ent = array (
		'&sbquo;' => "'",
		'&bdquo;' => '"',
		'&hellip;' => '...',
		'&euro;' => 'EURO',
		'&lsaquo;' => "'",
		'&lsquo;' => "'",
		'&rsquo;' => "'",
		'&ldquo;' => '"',
		'&rdquo;' => '"',
		'&bull;' => 'o',
		'&ndash;' => '-',
		'&mdash;' => '-',
		'&trade;' => '(TM)',
		'&rsaquo;' => "'",
		'&brvbar;' => '|',
		'&sect;' => 'S',
		'&laquo;' => '"',
		'&shy;' => '',
		'&reg;' => '(R)',
		'&plusmn;' => '+-',
		'&copy;'  => '(C)',
		'&#8470;' => 'N',
		'&raquo;' => '"',
		'&nbsp;'  => ' ',
		'&#8217;' => "'",
		'&#8211;' => '-',
		'&#8212;' => '-'
	);
	return strtr($s, $ent);
}

 /**
  * Qute and sometimes very helpful function from Ruby language
  * How it works - Input: "aabbbbbzzzzzzz   111", Output: "abz 1"
  *
  * @param  String  $inputString    String which need to squeeze
  * @param  String  $onlyTheseChars Simple regexp pattern, defines, which symbols need to squeeze, by default all. Example: "abc123", "a-z", "0-9a-f\n", etc.
  * @param  Boolean $caseMatters    Use caseless comparsion or not. By default, case is matter.
  * @return String
  */
function squeeze($inputString, $onlyTheseChars = null, $caseMatters = true){
	if(strlen($inputString) > 1){
		$pattern  = $onlyTheseChars ? '['.preg_quote($onlyTheseChars, '#').']' : '.';
		$caseFlag = $caseMatters    ? '' : 'i';
		$inputString = preg_replace("#({$pattern})(?=\\1)#{$caseFlag}",'',$inputString);
	}
	return $inputString;
}
 /**
  * Replace limited count of occurrences of the search string with the replacement string
  * @see _strReplaceLimited()
  */
function str_lreplace($search, $replacement, $subject, $limit = 1){
	return _strReplaceLimited($search, $replacement, $subject, $limit, true);
}

 /**
  * Case-insensitive version of str_lreplace()
  * @see _strReplaceLimited()
  */
function str_lireplace($search, $replacement, $subject, $limit = 1){
	return _strReplaceLimited($search, $replacement, $subject, $limit, false);
}
 /**
  * Realization of str_lreplace() and str_lireplace()
  * Unlike of str_[i]replace, first three args cannot be an arrays
  *
  * @param  String  $search      What  will be replaced
  * @param  String  $replacement That  we use for replacement
  * @param  String  $subject     Where we do search
  * @param  Integer $limit       The maximum possible replacements, by default 1
  * @param  Boolean $caseMatters Use caseless comparsion or not. By default, case is matter.
  * @return String
  */
  
function _strReplaceLimited($search, $replacement, $subject, $limit = 1, $caseMatters = true){
	$replaced = 0; $limit = (int)$limit; $len = strlen($search);
	if($caseMatters){
		$strp = 'strpos';
	} else {
		$strp = 'stripos';
	}
	
	while($replaced++ < $limit && ($pos = $strp($subject, $search)) !== false){
		$subject = substr_replace($subject, $replacement, $pos, $len);
	}
	return $subject;
}

function appendParamToUrl($url, $k, $v, $fragment = false, $num_save = false) {
	$tmp = parse_url($url);
	$url = NULL;
	if (!empty($tmp['scheme'])) {
		$url .= $tmp['scheme'].'://'.$tmp['host'];
	} else {
		$url .= PXRequest::GetHttpProto().'://'.PXRequest::GetHttpHost();
	}
	if (!empty($tmp['path'])) {
		if ($tmp['path']{0} == '/') {
			$url .= $tmp['path'];
		} else {
			$url .= '/'.$tmp['path'];
		}
	} else {
		$url .= '/';
	}
	if (!empty($tmp['query'])) {
		parse_str($tmp['query'], $qs);
	} else {
		$qs = array();
	}
	if ($fragment === false) $qs[$k] = $v;
	$url .= '?';
	foreach ($qs as $qk=>$qv) {
		if (is_array($qv)) {
			$url .= multiLevelVarsToString($qk, $qv, null, $num_save) . '&';
		} else {
			$url .= $qk.'='.urlencode($qv).'&';
		}
	}
	$url = substr($url, 0, -1);
	if ($fragment === false) {
		if (isset($tmp['fragment'])) $url .= '#' . $tmp['fragment'];
	} else {
		$url .= '#' . $fragment;
	}
	return $url;
}

function removeParamFromUrl($url, $k, $fragment = false, $removeAll = false, $num_save = false) {
	$tmp = parse_url($url);
	$url = NULL;
	if (!empty($tmp['scheme'])) {
		$url .= $tmp['scheme'].'://'.$tmp['host'];
	} else {
		$url .= PXRequest::GetHttpProto().'://'.PXRequest::GetHttpHost();
	}
	if (!empty($tmp['path'])) {
		if ($tmp['path']{0} == '/') {
			$url .= $tmp['path'];
		} else {
			$url .= '/'.$tmp['path'];
		}
	} else {
		$url .= '/';
	}
	if (!empty($tmp['query'])) {
		parse_str($tmp['query'], $qs);
	} else {
		$qs = array();
	}

	if($removeAll === true) {
		$qs = array();
	}

	if ($fragment === false and isset($qs[$k])) unset($qs[$k]);
	$url .= '?';
	foreach ($qs as $qk=>$qv) {
		if (is_array($qv)) {
			$url .= multiLevelVarsToString($qk, $qv, null, $num_save) . '&';
		} else {
			$url .= $qk.'='.urlencode($qv).'&';
		}
	}
	$url = substr($url, 0, -1);
	if ($fragment === false  and isset($tmp['fragment'])) $url .= '#' . $tmp['fragment'];
	return $url;
}

function multiLevelVarsToString($name, $arrayVars, $leaf=null, $num_save=false) {
	foreach($arrayVars as $k=>$a) {
		$t[$k] = "{$leaf}[".((is_numeric($k) && !$num_save) ? '' : $k)."]";
		$t[$k] = is_array($a) ? multiLevelVarsToString($name, $a, $t[$k], $num_save) : $name.$t[$k]."=".urlencode($a); 
	}

	return join ('&', $t);
}

function generateRandomString($length) {
	$tmp = "0123456789ABCDEF";
	$string = NULL;
	for($i=0;$i<$length;$i++) {
		$string .= $tmp{rand(0,15)};
	}
	return $string;
}

function isUtfString($str) {
	$len = strlen($str);
	for ($i = 0; $i < $len; $i++) {
		$cp = ord($str[$i]);
		if     ($cp >= 0x00 && $cp <= 0x7F) continue; # 00 - 7F -- 1 byte ascii
		elseif ($cp >= 0xC2 && $cp <= 0xDF) $n = 1;   # C2 - DF -- 2 bytes
		elseif (($cp & 0xF0) == 0xE0)       $n = 2;   # E0 - EF -- 3 bytes
		elseif ($cp >= 0xF0 && $cp <= 0xF4) $n = 3;   # F0 - F4 -- 4 bytes
		else return false;

		# length check
		if ($i + $n >= $len)
			return false;

		# hardcore
		for ($j = 0; $j < $n; $j++) {
			if ((ord($str[++$i]) & 0xC0) != 0x80)     # 80 - BF -- 2nd, 3rd, 4th bytes 
				return false;
		}
	}
	return true;
}

function isAsciiString($AStr) {
	static $ptrASCII = '[\x00-\x7F]';
	return preg_match("/^{$ptrASCII}*$/sS", $AStr);
}

function myconv($from, $to, $string, $be_quiet = true) {
	if(isAsciiString($string)) {
		return $string;
	}

	if(!function_exists('iconv')) {
		FatalError('Can\'t find ICONV');
	}

	// FIXME: it hack
	if (is_string($string) && isUtfString($string)) {
		$from = 'u';
	}

	static $entities = array(/*{{{*/
		160 => "&nbsp;",
		161 => "&iexcl;",
		162 => "&cent;",
		163 => "&pound;",
		164 => "&curren;",
		165 => "&yen;",
		166 => "&brvbar;",
		167 => "&sect;",
		168 => "&uml;",
		169 => "&copy;",
		170 => "&ordf;",
		171 => "&laquo;",
		172 => "&not;",
		173 => "&shy;",
		174 => "&reg;",
		175 => "&macr;",
		176 => "&deg;",
		177 => "&plusmn;",
		178 => "&sup2;",
		179 => "&sup3;",
		180 => "&acute;",
		181 => "&micro;",
		182 => "&para;",
		183 => "&middot;",
		184 => "&cedil;",
		185 => "&sup1;",
		186 => "&ordm;",
		187 => "&raquo;",
		188 => "&frac14;",
		189 => "&frac12;",
		190 => "&frac34;",
		191 => "&iquest;",
		192 => "&Agrave;",
		193 => "&Aacute;",
		194 => "&Acirc;",
		195 => "&Atilde;",
		196 => "&Auml;",
		197 => "&Aring;",
		198 => "&AElig;",
		199 => "&Ccedil;",
		200 => "&Egrave;",
		201 => "&Eacute;",
		202 => "&Ecirc;",
		203 => "&Euml;",
		204 => "&Igrave;",
		205 => "&Iacute;",
		206 => "&Icirc;",
		207 => "&Iuml;",
		208 => "&ETH;",
		209 => "&Ntilde;",
		210 => "&Ograve;",
		211 => "&Oacute;",
		212 => "&Ocirc;",
		213 => "&Otilde;",
		214 => "&Ouml;",
		215 => "&times;",
		216 => "&Oslash;",
		217 => "&Ugrave;",
		218 => "&Uacute;",
		219 => "&Ucirc;",
		220 => "&Uuml;",
		221 => "&Yacute;",
		222 => "&THORN;",
		223 => "&szlig;",
		224 => "&agrave;",
		225 => "&aacute;",
		226 => "&acirc;",
		227 => "&atilde;",
		228 => "&auml;",
		229 => "&aring;",
		230 => "&aelig;",
		231 => "&ccedil;",
		232 => "&egrave;",
		233 => "&eacute;",
		234 => "&ecirc;",
		235 => "&euml;",
		236 => "&igrave;",
		237 => "&iacute;",
		238 => "&icirc;",
		239 => "&iuml;",
		240 => "&eth;",
		241 => "&ntilde;",
		242 => "&ograve;",
		243 => "&oacute;",
		244 => "&ocirc;",
		245 => "&otilde;",
		246 => "&ouml;",
		247 => "&divide;",
		248 => "&oslash;",
		249 => "&ugrave;",
		250 => "&uacute;",
		251 => "&ucirc;",
		252 => "&uuml;",
		253 => "&yacute;",
		254 => "&thorn;",
		255 => "&yuml;",

		402 => "&fnof;",
		913 => "&Alpha;",
		914 => "&Beta;",
		915 => "&Gamma;",
		916 => "&Delta;",
		917 => "&Epsilon;",
		918 => "&Zeta;",
		919 => "&Eta;",
		920 => "&Theta;",
		921 => "&Iota;",
		922 => "&Kappa;",
		923 => "&Lambda;",
		924 => "&Mu;",
		925 => "&Nu;",
		926 => "&Xi;",
		927 => "&Omicron;",
		928 => "&Pi;",
		929 => "&Rho;",
		931 => "&Sigma;",
		932 => "&Tau;",
		933 => "&Upsilon;",
		934 => "&Phi;",
		935 => "&Chi;",
		936 => "&Psi;",
		937 => "&Omega;",
		945 => "&alpha;",
		946 => "&beta;",
		947 => "&gamma;",
		948 => "&delta;",
		949 => "&epsilon;",
		950 => "&zeta;",
		951 => "&eta;",
		952 => "&theta;",
		953 => "&iota;",
		954 => "&kappa;",
		955 => "&lambda;",
		956 => "&mu;",
		957 => "&nu;",
		958 => "&xi;",
		959 => "&omicron;",
		960 => "&pi;",
		961 => "&rho;",
		962 => "&sigmaf;",
		963 => "&sigma;",
		964 => "&tau;",
		965 => "&upsilon;",
		966 => "&phi;",
		967 => "&chi;",
		968 => "&psi;",
		969 => "&omega;",
		977 => "&thetasym;",
		978 => "&upsih;",
		982 => "&piv;",
		8226 => "&bull;",
		8230 => "&hellip;",
		8242 => "&prime;",
		8243 => "&Prime;",
		8254 => "&oline;",
		8260 => "&frasl;",
		8472 => "&weierp;",
		8465 => "&image;",
		8476 => "&real;",
		8482 => "&trade;",
		8501 => "&alefsym;",
		8592 => "&larr;",
		8593 => "&uarr;",
		8594 => "&rarr;",
		8595 => "&darr;",
		8596 => "&harr;",
		8629 => "&crarr;",
		8656 => "&lArr;",
		8657 => "&uArr;",
		8658 => "&rArr;",
		8659 => "&dArr;",
		8660 => "&hArr;",
		8704 => "&forall;",
		8706 => "&part;",
		8707 => "&exist;",
		8709 => "&empty;",
		8711 => "&nabla;",
		8712 => "&isin;",
		8713 => "&notin;",
		8715 => "&ni;",
		8719 => "&prod;",
		8721 => "&sum;",
		8722 => "&minus;",
		8727 => "&lowast;",
		8730 => "&radic;",
		8733 => "&prop;",
		8734 => "&infin;",
		8736 => "&ang;",
		8743 => "&and;",
		8744 => "&or;",
		8745 => "&cap;",
		8746 => "&cup;",
		8747 => "&int;",
		8756 => "&there4;",
		8764 => "&sim;",
		8773 => "&cong;",
		8776 => "&asymp;",
		8800 => "&ne;",
		8801 => "&equiv;",
		8804 => "&le;",
		8805 => "&ge;",
		8834 => "&sub;",
		8835 => "&sup;",
		8836 => "&nsub;",
		8838 => "&sube;",
		8839 => "&supe;",
		8853 => "&oplus;",
		8855 => "&otimes;",
		8869 => "&perp;",
		8901 => "&sdot;",
		8968 => "&lceil;",
		8969 => "&rceil;",
		8970 => "&lfloor;",
		8971 => "&rfloor;",
		9001 => "&lang;",
		9002 => "&rang;",
		9674 => "&loz;",
		9824 => "&spades;",
		9827 => "&clubs;",
		9829 => "&hearts;",
		9830 => "&diams;",

		338 => "&OElig;",
		339 => "&oelig;",
		352 => "&Scaron;",
		353 => "&scaron;",
		376 => "&Yuml;",
		710 => "&circ;",
		732 => "&tilde;",
		8194 => "&ensp;",
		8195 => "&emsp;",
		8201 => "&thinsp;",
		8204 => "&zwnj;",
		8205 => "&zwj;",
		8206 => "&lrm;",
		8207 => "&rlm;",
		8211 => "&ndash;",
		8212 => "&mdash;",
		8216 => "&lsquo;",
		8217 => "&rsquo;",
		8218 => "&sbquo;",
		8220 => "&ldquo;",
		8221 => "&rdquo;",
		8222 => "&bdquo;",
		8224 => "&dagger;",
		8225 => "&Dagger;",
		8240 => "&permil;",
		8249 => "&lsaquo;",
		8250 => "&rsaquo;",
		8364 => "&euro;",
	);/*}}}*/
	static $one2enc = array(/*{{{*/
		'k' => 'koi8-r',
		'w' => 'windows-1251',
		'i' => 'iso-8859-5',
		'a' => 'cp866',
		'd' => 'cp866',
		'm' => 'mac-cyrillic',
		'u' => 'utf-8',
	);/*}}}*/
	static $tables = array();

	if (array_key_exists($from, $one2enc)) $from = $one2enc[$from];
	if (array_key_exists($to,   $one2enc)) $to   = $one2enc[$to];

	// are charsets valid?
	$prev_track_errors = ini_get('track_errors');
	ini_set('track_errors', true);
	$php_errormsg = false;
	set_error_handler("_myconvErrorHandler", E_NOTICE);
	@iconv($from, $to, '');
	restore_error_handler();
	ini_set('track_errors', $prev_track_errors);
	if ($php_errormsg) {
		if(!$be_quiet) {
			trigger_error(__FUNCTION__ . '(): ' . $php_errormsg);
		}
		return false;
	}

	// maybe we can just use iconv?
	ini_set('track_errors', true);
	$php_errormsg = false;
	set_error_handler("_myconvErrorHandler", E_NOTICE);
	$res = @iconv($from, $to, $string);
	restore_error_handler();
	ini_set('track_errors', $prev_track_errors);
	if ($php_errormsg === false) {
		return $res;
	} else {
		unset($res);
	}

	if($from == $to) {
		return $string;
	}

	// check if string can be converted from $from encoding to ucs-2be
	ini_set('track_errors', true);
	$php_errormsg = false;
	set_error_handler("_myconvErrorHandler", E_NOTICE);
	@iconv($from, 'ucs-2be', $string);
	restore_error_handler();
	ini_set('track_errors', $prev_track_errors);
	if ($php_errormsg) {
		if (!$be_quiet) {
			trigger_error(__FUNCTION__ . '(): ' . $php_errormsg);
		}
		return false;
	}

	// test if input enc is one-byte chr(224) = cyrillic letter a-small
	$onebyte = (strlen(@iconv('windows-1251', $from, chr(224))) < 2);
	$key = $onebyte ? "$from~$to": $to;

	if (!array_key_exists($key, $tables)) {
		$fname = (defined('BASEPATH') ? BASEPATH . '/tmp/cache/myconv' : ini_get('upload_tmp_dir')) . '/recode-table-' . $key;
		$tables[$key] = array();

		if (is_file($fname)) {
			$tables[$key] = @unserialize(@file_get_contents($fname));
		}

		if (!$tables[$key]) {
			$tables[$key] = array();

			if ($onebyte) {
				for ($i = 0; $i <= 0xff; $i++) {
					$c = @iconv($from, $to, chr($i));
					if (strlen($c)) {
						$tables[$key][chr($i)] = $c;
					} else {
						$c = @iconv($from, 'ucs-2be', chr($i));
						if (strlen($c)) {
							$c = unpack('n', $c);
							$c = reset($c);
							$tables[$key][chr($i)] = (array_key_exists($c, $entities)) ? $entities[$c] : "&#$c;";
						} else {
							$tables[$key][chr($i)] = '';
						}
					}
				}
			} else {
				for ($i = 0; $i <= 0xffff; $i++) {
					$c = pack('n', $i);
					$tables[$key][$c] = (array_key_exists($i, $entities)) ? $entities[$i] : "&#$i;";
				}

				for ($i = 0; $i < 256; $i++) {
					$c = @iconv($to, 'ucs-2be', chr($i));
					if (!strlen($c)) continue;
					$tables[$key][$c] = chr($i);
				}
			}

			MakeDirIfNotExists(dirname($fname));
			if ($f = @fopen($fname, 'w')) {
				fwrite($f, serialize($tables[$key]));
				fclose($f);
			}
		}
	}

	return strtr($onebyte ? $string : iconv($from, 'ucs-2be', $string), $tables[$key]);
}

/*
 * (C) http://www.greywyvern.com/code/php/htmlwrap_1.1.php.txt
 */
function wordWrapHtmlSafe($str, $width = 60, $break = "\n", $nobreak = "", $nobr = "pre", $utf = false) {
	// Split HTML content into an array delimited by < and >
	// The flags save the delimeters and remove empty variables
	$content = preg_split("/([<>])/", $str, -1, PREG_SPLIT_DELIM_CAPTURE | PREG_SPLIT_NO_EMPTY);

	// Transform protected element lists into arrays
	$nobreak = explode(" ", $nobreak);
	$nobr    = explode(" ", $nobr);

	// Variable setup
	$intag = false;
	$innbk = array();
	$innbr = array();
	$drain = "";
	$utf = ($utf) ? "u" : "";

	// List of characters it is "safe" to insert line-breaks at
	// Do not add ampersand (&) as it will mess up HTML Entities
	// It is not necessary to add < and >
	$lbrks = "/?!%)-}]\\\"':;";

	// We use \r for adding <br /> in the right spots so just switch to \n
	if ($break == "\r") $break = "\n";

	while (list(, $value) = each($content)) {
		switch ($value) {

			// If a < is encountered, set the "in-tag" flag
			case "<": $intag = true; break;

			// If a > is encountered, remove the flag
			case ">": $intag = false; break;

			default:

				// If we are currently within a tag...
				if ($intag) {

					// If the first character is not a / then this is an opening tag
					if ($value{0} != "/") {

						// Collect the tag name
						preg_match("/^(.*?)(\s|$)/$utf", $value, $t);

						// If this is a protected element, activate the associated protection flag
						if ((!count($innbk) && in_array($t[1], $nobreak)) || in_array($t[1], $innbk)) $innbk[] = $t[1];
						if ((!count($innbr) && in_array($t[1], $nobr)) || in_array($t[1], $innbr)) $innbr[] = $t[1];

					// Otherwise this is a closing tag
					} else {

						// If this is a closing tag for a protected element, unset the flag
						if (in_array(substr($value, 1), $innbk)) unset($innbk[count($innbk)]);
						if (in_array(substr($value, 1), $innbr)) unset($innbr[count($innbr)]);
					}

				// Else if we're outside any tags...
				} else if ($value) {

					// If unprotected, remove all existing \r, replace all existing \n with \r
					if (!count($innbr)) $value = str_replace("\n", "\r", str_replace("\r", "", $value));

					// If unprotected, enter the line-break loop
					if (!count($innbk)) {
						do {
							$store = $value;

							// Find the first stretch of characters over the $width limit
							if (preg_match("/^(.*?\s|^)(([^\s&]|&(\w{2,5}|#\d{2,4});){".$width."})(?!(".preg_quote($break, "/")."|\s))(.*)$/s$utf", $value, $match)) {

								// Determine the last "safe line-break" character within this match
								for ($x = 0, $ledge = 0; $x < strlen($lbrks); $x++) $ledge = max($ledge, strrpos($match[2], $lbrks{$x}));
								if (!$ledge) $ledge = strlen($match[2]) - 1;

								// Insert the modified string
								$value = $match[1].substr($match[2], 0, $ledge + 1).$break.substr($match[2], $ledge + 1).$match[6];
							}

						// Loop while overlimit strings are still being found
						} while ($store != $value);
					}

					// If unprotected, replace all \r with <br />\n to finish
					if (!count($innbr)) $value = str_replace("\r", "<br />\n", $value);
				}
		}

		// Send the modified segment down the drain
		$drain .= $value;
	}

	// Return contents of the drain
	return $drain;
}

function fixWordLength($string, $length = 10) {
	$delim = chr(1);
	$doubleDelim = $delim . $delim;
	$vowel    = "[ÕÅÙÁÏÜÑÉÀaeiouy]";  // english vowels: a, e, i, o, u, y.
	$notvowel = "[^ÕÅÙÁÏÜÑÉÀaeiouy" . $delim ."]";
	$endPattern = '\s\d\?\)!,.\-;:\'"`';
	
	// replace htmlentities
	$entityPattern = '/(&#\d+;|&\w+;)/';
	$entityReplacement = chr(2);
	
	preg_match_all($entityPattern, $string, $replacement);
	$replacement = current($replacement);
	
	$string = preg_replace($entityPattern, $entityReplacement, $string);

	$hyphens = $string;
	$hyphens = preg_replace('/([\s,.-])/i', '$1' . $doubleDelim, $hyphens);
	$hyphens = preg_replace('/([ßØÊÙ])/i' , '$1' . $doubleDelim, $hyphens);

	$hyphens = preg_replace('/(' . $vowel .')(' . $vowel . ')([^' . $endPattern . '])/i',                             '$1' . $doubleDelim . '$2$3', $hyphens);
	$hyphens = preg_replace('/(' . $vowel    . $notvowel . ')(' . $notvowel . $vowel .')/i',                          '$1' . $delim .       '$2', $hyphens);
	$hyphens = preg_replace('/(' . $notvowel . $vowel    . ')(' . $notvowel . $vowel . ')/i',                         '$1' . $delim .       '$2', $hyphens);
	$hyphens = preg_replace('/(' . $vowel    . $notvowel . ')(' . $notvowel . $notvowel . $vowel . ')/i',             '$1' . $delim .       '$2', $hyphens);
	$hyphens = preg_replace('/(' . $vowel    . $notvowel . $notvowel . ')(' . $notvowel . $notvowel . $vowel . ')/i', '$1' . $delim .       '$2', $hyphens);
	
	// second time (to catch close standing syllables)
	$hyphens = preg_replace('/(' . $vowel .')(' . $vowel . ')([^' . $endPattern . '])/i',                             '$1' . $doubleDelim . '$2$3', $hyphens);
	$hyphens = preg_replace('/(' . $vowel    . $notvowel . ')(' . $notvowel . $vowel . ')/i',                         '$1' . $delim .       '$2', $hyphens);
	$hyphens = preg_replace('/(' . $notvowel . $vowel    . ')(' . $notvowel . $vowel . ')/i',                         '$1' . $delim .       '$2', $hyphens);
	$hyphens = preg_replace('/(' . $vowel    . $notvowel . ')(' . $notvowel . $notvowel . $vowel . ')/i',             '$1' . $delim .       '$2', $hyphens);
	$hyphens = preg_replace('/(' . $vowel    . $notvowel . $notvowel . ')(' . $notvowel . $notvowel . $vowel . ')/i', '$1' . $delim .       '$2', $hyphens);
	
	$syllables = explode($delim, $hyphens);

	$string = $tmp = array_shift($syllables);

	foreach ($syllables as $s) {
		$tmp .= $s;
		$end = substr($string, -1);

		if (strlen(trim($tmp)) > $length || $end == ' ') {
			$tmp = $s;
			if (!preg_match("/([{$endPattern}])/i", $end)) {
				$string .= "&shy;";
			}
		}
		
		$string .= $s;
	}
	
	// get back htmlentities
	$pieces = explode($entityReplacement, $string);
	foreach($pieces as $key => $piece){
		if (isset($replacement[$key])) {
			$pieces[$key] = $piece . $replacement[$key];
		}
	}
	$string = implode($pieces);
	
	return $string;
}

// dirty hack
function _myconvErrorHandler($error_code, $error_msg) {
	return false;
}

/**
* Decode one %uXXXX entity (RE callback).
*/
function utf8_urldecodeCallback($p) {
	static $_encTables = array(
		0x2500, 0x2502, 0x250C, 0x2510, 0x2514, 0x2518, 0x251C, 0x2524,
		0x252C, 0x2534, 0x253C, 0x2580, 0x2584, 0x2588, 0x258C, 0x2590,
		0x2591, 0x2592, 0x2593, 0x2320, 0x25A0, 0x2219, 0x221A, 0x2248,
		0x2264, 0x2265, 0x00A0, 0x2321, 0x00B0, 0x00B2, 0x00B7, 0x00F7,
		0x2550, 0x2551, 0x2552, 0x0451, 0x2553, 0x2554, 0x2555, 0x2556,
		0x2557, 0x2558, 0x2559, 0x255A, 0x255B, 0x255C, 0x255d, 0x255E,
		0x255F, 0x2560, 0x2561, 0x0401, 0x2562, 0x2563, 0x2564, 0x2565,
		0x2566, 0x2567, 0x2568, 0x2569, 0x256A, 0x256B, 0x256C, 0x00A9,
		0x044E, 0x0430, 0x0431, 0x0446, 0x0434, 0x0435, 0x0444, 0x0433,
		0x0445, 0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043d, 0x043E,
		0x043F, 0x044F, 0x0440, 0x0441, 0x0442, 0x0443, 0x0436, 0x0432,
		0x044C, 0x044B, 0x0437, 0x0448, 0x044d, 0x0449, 0x0447, 0x044A,
		0x042E, 0x0410, 0x0411, 0x0426, 0x0414, 0x0415, 0x0424, 0x0413,
		0x0425, 0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041d, 0x041E,
		0x041F, 0x042F, 0x0420, 0x0421, 0x0422, 0x0423, 0x0416, 0x0412,
		0x042C, 0x042B, 0x0417, 0x0428, 0x042d, 0x0429, 0x0427, 0x042A      
	);

	$hex = $p[1];
	$dec = hexdec($hex);

	if (is_callable('iconv')) {
		$c = @iconv('UCS-2BE', 'KOI8-R', pack('n', $dec));

	} else {
		if ($dec < 128) {
			$c = chr($dec);

		} else {
			$p = array_search($dec, $encTables);

			if ($p !== false) {
				$c = chr(128 + $p);
			} else {
				$c = "";
			}
		}
	}

	if (!strlen($c)) {
		$c = '?';
	}

	return $c;
}

if (!function_exists("lcfirst")) {
	function lcfirst($string) {
		$string{0} = strtolower($string{0});
		return $string;
	}
}

?>
