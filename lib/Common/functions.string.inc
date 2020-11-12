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

function NumericCmp($a, $b) {
	if ($a == $b) return 0;
	return ($a < $b) ? -1 : 1;
}


function _StrToLower($text) {
	$text  = mb_strtolower($text);
	if (defined('MB_DISABLED')) {
		$trans = ['ю' => 'Ю', 'а' => 'А', 'б' => 'Б', 'ц' => 'Ц', 'д' => 'Д', 'е' => 'Е', 'ё' => 'Ё', 'ф' => 'Ф', 'г' => 'Г', 'х' => 'Х', 'и' => 'И', 'й' => 'Й', 'к' => 'К', 'л' => 'Л', 'м' => 'М', 'н' => 'Н', 'о' => 'О', 'п' => 'П', 'я' => 'Я', 'р' => 'Р', 'с' => 'С', 'т' => 'Т', 'у' => 'У', 'ж' => 'Ж', 'в' => 'В', 'ь' => 'Ь', 'ы' => 'Ы', 'з' => 'З', 'ш' => 'Ш', 'э' => 'Э', 'щ' => 'Щ', 'ч' => 'Ч', 'ъ' => 'Ъ'];
		$trans = array_flip($trans);
		$text  = mb_strtr($text, $trans);
	}
	return $text;
}

function _StrToUpper($text) {
	$text  = mb_strtoupper($text);
	if (defined('MB_DISABLED')) {
		$trans = ['ю' => 'Ю', 'а' => 'А', 'б' => 'Б', 'ц' => 'Ц', 'д' => 'Д', 'е' => 'Е', 'Ё' => 'ё', 'ф' => 'Ф', 'г' => 'Г', 'х' => 'Х', 'и' => 'И', 'й' => 'Й', 'к' => 'К', 'л' => 'Л', 'м' => 'М', 'н' => 'Н', 'о' => 'О', 'п' => 'П', 'я' => 'Я', 'р' => 'Р', 'с' => 'С', 'т' => 'Т', 'у' => 'У', 'ж' => 'Ж', 'в' => 'В', 'ь' => 'Ь', 'ы' => 'Ы', 'з' => 'З', 'ш' => 'Ш', 'э' => 'Э', 'щ' => 'Щ', 'ч' => 'Ч', 'ъ' => 'Ъ'];
		$text  = mb_strtr($text, $trans);
	}
	return $text;
}

function _Translit($text) {
	$trans = [
		'А' => 'A',  'Б' => 'B',  'В' => 'V',   'Г' => 'G', 'Д' => 'D',  'Е' => 'E',
		'Ё' => 'E',  'Ж' => 'G',  'З' => 'ZH',  'И' => 'I', 'Й' => 'JO', 'К' => 'K',
		'Л' => 'L',  'М' => 'M',  'Н' => 'N',   'О' => 'O', 'П' => 'P',  'Р' => 'R',
		'С' => 'S',  'Т' => 'T',  'У' => 'U',   'Ф' => 'F', 'Х' => 'H',  'Ц' => 'TS',
		'Ч' => 'CH', 'Ш' => 'SH', 'Щ' => 'SCH', 'Ъ' => '',  'Ы' => 'Y',  'Ь' => '',
		'Э' => 'E',  'Ю' => 'JU', 'Я' => 'JA',
		'а' => 'a',  'б' => 'b',  'в' => 'v',   'г' => 'g', 'д' => 'd',  'е' => 'e',
		'ё' => 'jo', 'ж' => 'zh', 'з' => 'z',   'и' => 'i', 'й' => 'jo', 'к' => 'k',
		'л' => 'l',  'м' => 'm',  'н' => 'n',   'о' => 'o', 'п' => 'p',  'р' => 'r',
		'с' => 's',  'т' => 't',  'у' => 'y',   'ф' => 'f', 'х' => 'h',  'ц' => 'ts',
		'ч' => 'ch', 'ш' => 'sh', 'щ' => 'sch', 'ъ' => '',  'ы' => 'y',  'ь' => '',
		'э' => 'e',  'ю' => 'yu', 'я' => 'ya'
	];
	$text = mb_strtr($text, $trans);
	return $text;
}

function _TranslitFilename($text) {
	$trans = [
		'А' => 'a',     'а' => 'a',     'Б' => 'b',     'б' => 'b',
		'В' => 'v',     'в' => 'v',     'Г' => 'g',     'г' => 'g',
		'Д' => 'd',     'д' => 'd',     'Е' => 'e',     'е' => 'e',     'Ё' => 'e',     'ё' => 'e',
		'Ж' => 'zh',    'ж' => 'zh',    'З' => 'z',     'з' => 'z',
		'И' => 'i',     'и' => 'i',     'Й' => 'j',     'й' => 'j',
		'К' => 'k',     'к' => 'k',     'Л' => 'l',     'л' => 'l',
		'М' => 'm',     'м' => 'm',     'Н' => 'n',     'н' => 'n',
		'О' => 'o',     'о' => 'o',     'П' => 'p',     'п' => 'p',
		'Р' => 'r',     'р' => 'r',     'С' => 's',     'с' => 's',
		'Т' => 't',     'т' => 't',     'У' => 'u',     'у' => 'u',
		'Ф' => 'f',     'ф' => 'f',     'Х' => 'h',     'х' => 'h',
		'Ц' => 'c',     'ц' => 'c',     'Ч' => 'ch',    'ч' => 'ch',
		'Ш' => 'sh',    'ш' => 'sh',    'Щ' => 'sch',   'щ' => 'sch',
		'Ъ' => '',      'ъ' => '',      'Ы' => 'i',     'ы' => 'i',
		'Ь' => '',      'ь' => '',      'Э' => 'e',     'э' => 'e',
		'Ю' => 'u',     'ю' => 'u',     'Я' => 'ya',    'я' => 'ya',

		' ' => '_',     ',' => '_',     '?' => '_',
		'&' => '_',     '#' => '_',     '$' => '_',     '%' => '_',
		'@' => '_',     '!' => '_',     '~' => '_',     '`' => '_',
		':' => '_',     '"' => '_',     '\'' => '_',    '\\' => '_',
		'/' => '_',     '<' => '_',     '>' => '_',     '^' => '_',
		'*' => '_',     '(' => '_',     ')' => '_',     '+' => '_',
		'|' => '_',     ';' => '_',     '=' => '_',     '[' => '_',
		']' => '_',     '{' => '_',     '}' => '_',

		"\n" => ''
	];

	$text = mb_strtr(trim($text), $trans);

	// Remove remaining unsafe characters.
	$text = preg_replace('![^0-9A-Za-z_.-]!', '', $text);
	// Remove multiple consecutive non-alphabetical characters.
	$text = preg_replace('/(_)_+|(\.)\.+|(-)-+/', '\\1\\2\\3', $text);
	// Check if filename is empty or starts with dot
	if (empty($text) || mb_strpos($text, '.') === 0) {
		$text = time() . $text;
	}

	return mb_substr($text, 0, 255); //max filename length in most common filesystems
}

function _TranslitUnique($text) {
	$trans = [
		'А' => 'A',   'Б' => 'B',  'В' => 'V',   'Г' => 'G', 'Д' => 'D',  'Е' => 'E',
		'Ё' => 'EO',  'Ж' => 'ZH', 'З' => 'Z',   'И' => 'I', 'Й' => 'JO', 'К' => 'K',
		'Л' => 'L',   'М' => 'M',  'Н' => 'N',   'О' => 'O', 'П' => 'P',  'Р' => 'R',
		'С' => 'S',   'Т' => 'T',  'У' => 'U',   'Ф' => 'F', 'Х' => 'H',  'Ц' => 'TS',
		'Ч' => 'CH',  'Ш' => 'SH', 'Щ' => 'SCH', 'Ъ' => '',  'Ы' => 'Y',  'Ь' => '',
		'Э' => 'JE',  'Ю' => 'JU', 'Я' => 'JA',
		'а' => 'a',   'б' => 'b',  'в' => 'v',   'г' => 'g', 'д' => 'd',  'е' => 'e',
		'ё' => 'jo',  'ж' => 'zh', 'з' => 'z',   'и' => 'i', 'й' => 'jo', 'к' => 'k',
		'л' => 'l',   'м' => 'm',  'н' => 'n',   'о' => 'o', 'п' => 'p',  'р' => 'r',
		'с' => 's',   'т' => 't',  'у' => 'y',   'ф' => 'f', 'х' => 'h',  'ц' => 'ts',
		'ч' => 'ch',  'ш' => 'sh', 'щ' => 'sch', 'ъ' => '',  'ы' => 'y',  'ь' => '',
		'э' => 'je',  'ю' => 'yu', 'я' => 'ya'
	];
	$text = mb_strtr($text, $trans);
	return $text;
}

function _UnTranslitUnique($text) {
	$trans = array_flip([
		'А' => 'A',   'Б' => 'B',  'В' => 'V',   'Г' => 'G', 'Д' => 'D',  'Е' => 'E',
		'Ё' => 'EO',  'Ж' => 'ZH',  'З' => 'Z',  'И' => 'I', 'Й' => 'JO', 'К' => 'K',
		'Л' => 'L',   'М' => 'M',  'Н' => 'N',   'О' => 'O', 'П' => 'P',  'Р' => 'R',
		'С' => 'S',   'Т' => 'T',  'У' => 'U',   'Ф' => 'F', 'Х' => 'H',  'Ц' => 'TS',
		'Ч' => 'CH',  'Ш' => 'SH', 'Щ' => 'SCH', 'Ы' => 'Y',
		'Э' => 'JE',  'Ю' => 'JU', 'Я' => 'JA',
		'а' => 'a',   'б' => 'b',  'в' => 'v',   'г' => 'g', 'д' => 'd',  'е' => 'e',
		'ё' => 'jo',  'ж' => 'zh', 'з' => 'z',   'и' => 'i', 'й' => 'jo', 'к' => 'k',
		'л' => 'l',   'м' => 'm',  'н' => 'n',   'о' => 'o', 'п' => 'p',  'р' => 'r',
		'с' => 's',   'т' => 't',  'у' => 'y',   'ф' => 'f', 'х' => 'h',  'ц' => 'ts',
		'ч' => 'ch',  'ш' => 'sh', 'щ' => 'sch', 'ы' => 'y',
		'э' => 'je',  'ю' => 'yu', 'я' => 'ya'
	]);
	$text = mb_strtr($text, $trans);
	return $text;
}

function _TranslitModern($text) {
	$trans = [
		'А' => 'A',  'Б' => 'B',  'В' => 'V',   'Г' => 'G', 'Д' => 'D',  'Е' => 'E',
		'Ё' => 'E',  'Ж' => 'Zh', 'З' => 'Z',   'И' => 'I', 'Й' => 'Y',  'К' => 'K',
		'Л' => 'L',  'М' => 'M',  'Н' => 'N',   'О' => 'O', 'П' => 'P',  'Р' => 'R',
		'С' => 'S',  'Т' => 'T',  'У' => 'U',   'Ф' => 'F', 'Х' => 'H',  'Ц' => 'Ts',
		'Ч' => 'Ch', 'Ш' => 'Sh', 'Щ' => 'Sch', 'Ъ' => '',  'Ы' => 'Y',  'Ь' => '',
		'Э' => 'E',  'Ю' => 'Yu', 'Я' => 'Ya',

		'а' => 'a',  'б' => 'b',  'в' => 'v',   'г' => 'g', 'д' => 'd',  'е' => 'e',
		'ё' => 'e',  'ж' => 'zh', 'з' => 'z',   'и' => 'i', 'й' => 'y',  'к' => 'k',
		'л' => 'l',  'м' => 'm',  'н' => 'n',   'о' => 'o', 'п' => 'p',  'р' => 'r',
		'с' => 's',  'т' => 't',  'у' => 'u',   'ф' => 'f', 'х' => 'h',  'ц' => 'ts',
		'ч' => 'ch', 'ш' => 'sh', 'щ' => 'sch', 'ъ' => '',  'ы' => 'y',  'ь' => '',
		'э' => 'e',  'ю' => 'yu', 'я' => 'ya',  "\xbf" => 'N'
	];
	$text = mb_strtr($text, $trans);
	return $text;
}

function NumericEndingsRussian($count, $one, $two, $zero) {
	$i = (int) mb_substr( $count, -1 );
	$s = (int) $count;
	$x = $s;
	if ( $x > 99 ) {
		$x = mb_substr( $s, -2 );
	}
	if ( $x >= 5 && $x <= 20 || $i >=5 || $i == 0 ) {
		return $zero;
	}
	else if ( $i >= 2 && $i <=4 ) {
		return $two;
	}
	return $one;
}

function htmlspecialcharsArray($array) {
	if (!is_array($array)) return [];
	foreach($array as $k=>$v) {
		if (is_array($v)) {
			$array[$k] = htmlspecialcharsArray($v);
		} else {
			$array[$k] = htmlspecialchars($v, ENT_COMPAT|ENT_HTML401, DEFAULT_CHARSET);
		}
	}
	return $array;
}

function DbQuoteString($s) {
	return "'".addslashes($s)."'";
}

function Strip1251($s) {
	static $ent = [
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
	];
	return mb_strtr($s, $ent);
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
	if(mb_strlen($inputString) > 1){
		$pattern  = $onlyTheseChars ? '['.preg_quote($onlyTheseChars, '#').']' : '.';
		$caseFlag = $caseMatters    ? '' : 'i';
		$inputString = preg_replace("#({$pattern})(?=\\1)#{$caseFlag}".REGEX_MOD,'',$inputString);
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
	$replaced = 0; $limit = (int)$limit; $len = mb_strlen($search);
	if($caseMatters){
		$strp = 'mb_strpos';
	} else {
		$strp = 'mb_stripos';
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
		parseStrMagic($tmp['query'], $qs);
	} else {
		$qs = [];
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
	$url = mb_substr($url, 0, -1);
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
		parseStrMagic($tmp['query'], $qs);
	} else {
		$qs = [];
	}

	if($removeAll === true) {
		$qs = [];
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
	$url = mb_substr($url, 0, -1);
	if ($fragment === false  and isset($tmp['fragment'])) $url .= '#' . $tmp['fragment'];
	return $url;
}

function parseStrMagic($str, &$arr) {
	mb_parse_str($str, $arr);
	if (get_magic_quotes_gpc()) {
		$arr = stripSlashesDeep($arr);
	}
}

function stripSlashesDeep($value) {
	return is_array($value) ? array_map("stripSlashesDeep", $value) : stripslashes($value);
}

function multiLevelVarsToString($name, $arrayVars, $leaf=null, $num_save=false) {
	foreach($arrayVars as $k=>$a) {
		$t[$k] = "{$leaf}[".((is_numeric($k) && !$num_save) ? '' : $k)."]";
		$t[$k] = is_array($a) ? multiLevelVarsToString($name, $a, $t[$k], $num_save) : $name.$t[$k]."=".urlencode($a);
	}

	return !empty($t) ? join ('&', $t) : '';
}

// NOTICE: no need here to use mb_ functions here
function generateRandomString($length) {
	$tmp = "0123456789ABCDEF";
	$string = NULL;
	for($i=0;$i<$length;$i++) {
		$string .= $tmp{rand(0,15)};
	}
	return $string;
}

// NOTICE: no need here to use mb_ functions here
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

// no need here to use preg_ with UTF-8 modifier
function isAsciiString($AStr) {
	static $ptrASCII = '[\x00-\x7F]';
	return (bool)preg_match("/^{$ptrASCII}*$/sS", $AStr);
}

/*
 * Substring for a UTF8 string.
 *
 * Input string should be UTF8-encoded.
 * - If an encoding mistake is found in the first '$from' UTF8 characters,
 * blank string ('') is returned.
 * - If an encoding mistake is found in the next charaters, only first N
 * correctly-encoded characters (starting from '$from') are returned
 * (N <= $length).
 *
 * The function returns always a correct UTF8-encoded string.
 */
// NOTICE: no need to use mb_ functions here
function substrUTF8($str, $from, $length) {
	$len = strlen($str);
	$chars = 0;
	$i = 0;
	$out = '';
	while ($chars < $from + $length and $i < $len) {
		$code = ord($str[$i]);
		if ($code >= 0x00 && $code <= 0x7F) {
			if ($chars >= $from) $out .= $str[$i];
			$i += 1;
		} elseif ($code >= 0xC0 && $code <= 0xDF) {
			if ($i + 1 >= $len) return $out;
			$code2 = ord($str[$i + 1]);
			if ($code2 < 0x80 || $code2 > 0xBF) return $out;
			if ($chars >= $from) $out .= $str[$i] . $str[$i + 1];
			$i += 2;
		} elseif ($code >= 0xE0 && $code <= 0xEF) {
			if ($i + 2 >= $len) return $out;
			$code2 = ord($str[$i + 1]);
			if ($code2 < 0x80 || $code2 > 0xBF) return $out;
			$code3 = ord($str[$i + 2]);
			if ($code3 < 0x80 || $code3 > 0xBF) return $out;
			if ($chars >= $from) $out .= $str[$i] . $str[$i + 1] . $str[$i + 2];
			$i += 3;
		} elseif ($code >= 0xF0 && $code <= 0xF7) {
			if ($i + 3 >= $len) return $out;
			$code2 = ord($str[$i + 1]);
			if ($code2 < 0x80 || $code2 > 0xBF) return $out;
			$code3 = ord($str[$i + 2]);
			if ($code3 < 0x80 || $code3 > 0xBF) return $out;
			$code4 = ord($str[$i + 3]);
			if ($code4 < 0x80 || $code4 > 0xBF) return $out;
			if ($chars >= $from) $out .= $str[$i] . $str[$i + 1] . $str[$i + 2] . $str[$i + 3];
			$i += 4;
		} else {
			return $out;
		}
		++$chars;
	}
	return $out;
}

/*
 * (C) http://www.greywyvern.com/code/php/htmlwrap_1.1.php.txt
 */
function wordWrapHtmlSafe($str, $width = 60, $break = "\n", $nobreak = "", $nobr = "pre", $utf = false) {
	// Split HTML content into an array delimited by < and >
	// The flags save the delimeters and remove empty variables
	$content = preg_split("/([<>])/".REGEX_MOD, $str, -1, PREG_SPLIT_DELIM_CAPTURE | PREG_SPLIT_NO_EMPTY);

	// Transform protected element lists into arrays
	$nobreak = explode(" ", $nobreak);
	$nobr    = explode(" ", $nobr);

	// Variable setup
	$intag = false;
	$innbk = [];
	$innbr = [];
	$drain = "";
	$utf = ($utf) ? "u" : "";

	// List of characters it is "safe" to insert line-breaks at
	// Do not add ampersand (&) as it will mess up HTML Entities
	// It is not necessary to add < and >
	$lbrks = "/?!%)-}]\\\"':;";

	// We use \r for adding <br /> in the right spots so just switch to \n
	if ($break == "\r") $break = "\n";

	foreach ($content as $_ => $value) {
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
						if (in_array(mb_substr($value, 1), $innbk)) unset($innbk[count($innbk)]);
						if (in_array(mb_substr($value, 1), $innbr)) unset($innbr[count($innbr)]);
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
								for ($x = 0, $ledge = 0; $x < mb_strlen($lbrks); $x++) $ledge = max($ledge, mb_strrpos($match[2], $lbrks{$x}));
								if (!$ledge) $ledge = mb_strlen($match[2]) - 1;

								// Insert the modified string
								$value = $match[1].mb_substr($match[2], 0, $ledge + 1).$break.mb_substr($match[2], $ledge + 1).$match[6];
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
	$vowel    = "[уеыаоэяиюaeiouy]";  // english vowels: a, e, i, o, u, y.
	$notvowel = "[^уеыаоэяиюaeiouy" . $delim ."]";
	$endPattern = '\s\d\?\)!,.\-;:\'"`';

	// replace htmlentities
	$entityPattern = '/(&#\d+;|&\w+;)/'.REGEX_MOD;
	$entityReplacement = chr(2);

	preg_match_all($entityPattern, $string, $replacement);
	$replacement = current($replacement);

	$string = preg_replace($entityPattern, $entityReplacement, $string);

	$hyphens = $string;
	$hyphens = preg_replace('/([\s,.-])/i'.REGEX_MOD, '$1' . $doubleDelim, $hyphens);
	$hyphens = preg_replace('/([ъьйы])/i'.REGEX_MOD , '$1' . $doubleDelim, $hyphens);

	$hyphens = preg_replace('/(' . $vowel .')(' . $vowel . ')([^' . $endPattern . '])/i'.REGEX_MOD,								'$1' . $doubleDelim . '$2$3', $hyphens);
	$hyphens = preg_replace('/(' . $vowel    . $notvowel . ')(' . $notvowel . $vowel .')/i'.REGEX_MOD,							'$1' . $delim .       '$2', $hyphens);
	$hyphens = preg_replace('/(' . $notvowel . $vowel    . ')(' . $notvowel . $vowel . ')/i'.REGEX_MOD,							'$1' . $delim .       '$2', $hyphens);
	$hyphens = preg_replace('/(' . $vowel    . $notvowel . ')(' . $notvowel . $notvowel . $vowel . ')/i'.REGEX_MOD,				'$1' . $delim .       '$2', $hyphens);
	$hyphens = preg_replace('/(' . $vowel    . $notvowel . $notvowel . ')(' . $notvowel . $notvowel . $vowel . ')/i'.REGEX_MOD,	'$1' . $delim .       '$2', $hyphens);

	// second time (to catch close standing syllables)
	$hyphens = preg_replace('/(' . $vowel .')(' . $vowel . ')([^' . $endPattern . '])/i'.REGEX_MOD,                             '$1' . $doubleDelim . '$2$3', $hyphens);
	$hyphens = preg_replace('/(' . $vowel    . $notvowel . ')(' . $notvowel . $vowel . ')/i'.REGEX_MOD,                         '$1' . $delim .       '$2', $hyphens);
	$hyphens = preg_replace('/(' . $notvowel . $vowel    . ')(' . $notvowel . $vowel . ')/i'.REGEX_MOD,                         '$1' . $delim .       '$2', $hyphens);
	$hyphens = preg_replace('/(' . $vowel    . $notvowel . ')(' . $notvowel . $notvowel . $vowel . ')/i'.REGEX_MOD,             '$1' . $delim .       '$2', $hyphens);
	$hyphens = preg_replace('/(' . $vowel    . $notvowel . $notvowel . ')(' . $notvowel . $notvowel . $vowel . ')/i'.REGEX_MOD, '$1' . $delim .       '$2', $hyphens);

	$syllables = explode($delim, $hyphens);

	$string = $tmp = array_shift($syllables);

	foreach ($syllables as $s) {
		$tmp .= $s;
		$end = mb_substr($string, -1);

		if (mb_strlen(trim($tmp)) > $length || $end == ' ') {
			$tmp = $s;
			if (!preg_match("/([{$endPattern}])/i".REGEX_MOD, $end)) {
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

/**
* Decode one %uXXXX entity (RE callback).
*/
function utf8_urldecode($p) {
	static $encTables = [
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
	];

	$hex = $p[1];
	$dec = hexdec($hex);

	if (is_callable('iconv')) {
		$c = @iconv(CHARSET_UCS2BE, DEFAULT_CHARSET, pack('n', $dec));

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

function quot($string, $double = true) {
	return mb_strtr($string, $double ? ['"' => '&quot;'] : ["'" => "&#039;"]);
}

/**
 * Сумма прописью
 * @author runcore
 */
function money2str($money, $stripkop=false) {
	$nol = 'ноль';
	$str[100]= ['','сто','двести','триста','четыреста','пятьсот','шестьсот', 'семьсот', 'восемьсот','девятьсот'];
	$str[11] = ['','десять','одиннадцать','двенадцать','тринадцать', 'четырнадцать','пятнадцать','шестнадцать','семнадцать', 'восемнадцать','девятнадцать','двадцать'];
	$str[10] = ['','десять','двадцать','тридцать','сорок','пятьдесят', 'шестьдесят','семьдесят','восемьдесят','девяносто'];
	$sex = [
		['','один','два','три','четыре','пять','шесть','семь', 'восемь','девять'],// m
		['','одна','две','три','четыре','пять','шесть','семь', 'восемь','девять'] // f
	];
	$forms = [
		['копейка', 'копейки', 'копеек', 1], // 10^-2
		['рубль', 'рубля', 'рублей',  0], // 10^ 0
		['тысяча', 'тысячи', 'тысяч', 1], // 10^ 3
		['миллион', 'миллиона', 'миллионов',  0], // 10^ 6
		['миллиард', 'миллиарда', 'миллиардов',  0], // 10^ 9
		['триллион', 'триллиона', 'триллионов',  0], // 10^12
	];
	$out = $tmp = [];
	$o = [];
	// Поехали!
	$tmp = explode('.', str_replace(',','.', $money));
	$rub = number_format($tmp[ 0], 0,'','-');
	if ($rub== 0) $out[] = $nol;
	// нормализация копеек
	$kop = isset($tmp[1]) ? mb_substr(mb_str_pad($tmp[1], 2, '0', STR_PAD_RIGHT), 0,2) : '00';
	$segments = explode('-', $rub);
	$offset = sizeof($segments);
	if ((int)$rub== 0) { // если 0 рублей
		$o[] = $nol;
		$o[] = NumericEndingsRussian( 0, $forms[1][ 0],$forms[1][1],$forms[1][2]);
	}
	else {
		foreach ($segments as $k=>$lev) {
			$sexi= (int) $forms[$offset][3]; // определяем род
			$ri = (int) $lev; // текущий сегмент
			if ($ri== 0 && $offset>1) {// если сегмент==0 & не последний уровень(там Units)
				$offset--;
				continue;
			}
			// нормализация
			$ri = mb_str_pad($ri, 3, '0', STR_PAD_LEFT);
			// получаем циферки для анализа
			$r1 = (int)mb_substr($ri, 0,1); //первая цифра
			$r2 = (int)mb_substr($ri,1,1); //вторая
			$r3 = (int)mb_substr($ri,2,1); //третья
			$r22= (int)$r2.$r3; //вторая и третья
			// разгребаем порядки
			if ($ri>99) $o[] = $str[100][$r1]; // Сотни
			if ($r22>20) {// >20
				$o[] = $str[10][$r2];
				$o[] = $sex[ $sexi ][$r3];
			}
			else { // <=20
				if ($r22>9) $o[] = $str[11][$r22-9]; // 10-20
				elseif($r22> 0) $o[] = $sex[ $sexi ][$r3]; // 1-9
			}
			// Рубли
			$o[] = NumericEndingsRussian($ri, $forms[$offset][ 0],$forms[$offset][1],$forms[$offset][2]);
			$offset--;
		}
	}
	// Копейки
	if (!$stripkop) {
		$o[] = $kop;
		$o[] = NumericEndingsRussian($kop,$forms[ 0][ 0],$forms[ 0][1],$forms[ 0][2]);
	}
	return preg_replace("/\s{2,}/".REGEX_MOD, ' ', implode(' ', $o));
}

// Converts "abc_def", "abc-def", "abcDef" strings to "AbcDef", but not "ab-cDef"
function convertStringToPascalCase ($s) {
	return ucfirst(convertStringToCamelCase($s));
}

// Converts "AbcDef" string to "abcDef"
function convertStringToCamelCase ($s) {
	return preg_replace_callback('/[\-_][a-z]/i'.REGEX_MOD, '__convertStringToCamelCase_helper', $s);
}
function __convertStringToCamelCase_helper($a) { return mb_strtoupper($a[0][1]); }

// Converts "AbcDef" string to "abc_def"
function convertStringToUnderscored ($s) {
	return ltrim(preg_replace_callback('/(?:[A-Z]|(?:-)[a-z])/'.REGEX_MOD, '__convertStringToUnderscored_helper', $s), '_');
}
function __convertStringToUnderscored_helper($a) { return '_' . mb_strtolower(substr($a[0], -1, 1)); }

// Converts "AbcDef" string to "abc-def"
function convertStringToDashed ($s) {
	return ltrim(preg_replace_callback('/(?:[A-Z]|(?:_)[a-z])/'.REGEX_MOD, '__convertStringToDashed_helper', $s), '-');
}
function __convertStringToDashed_helper($a) { return '-' . mb_strtolower(substr($a[0], -1, 1)); }

function _stripBadFileChars($fname) {
	return str_replace(['..' . DIRECTORY_SEPARATOR, DIRECTORY_SEPARATOR, chr(0)], '', $fname);
}

function softcut ($string, $max_length, $tail = null) {
	if (mb_strlen($string) < $max_length) {
		return $string;
	}
	if (is_null($tail)) {
		$tail = '...';
	}

	$tail_length = mb_strlen($tail);
	$swot = mb_substr($string, 0, $max_length - $tail_length); // string that shorter than max_length without tail width

	$cut_pos = 0;
	$delimiters = [' ', ',', '.', '"', "'", '/', ';', '-', '+', "\t"];
	foreach ($delimiters as $d) {
		$cut_pos = max($cut_pos, mb_strrpos($swot,$d));
	}

	$string = mb_substr($string, 0, $cut_pos);
	return $string . $tail;
}
