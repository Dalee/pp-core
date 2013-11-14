<?php
/*
Константы для удобства. Коды соответствуют символам в win-кодировке, но это
"совпадение":) - на самом деле, скрипт кросскодировочный (win и koi).
*/
if ((DEFAULT_CHARSET === CHARSET_KOI8R) || (DEFAULT_CHARSET === CHARSET_WINDOWS)) {
	define("TAG1",   "\xAC");
	define("TAG2",   "\xAD");
	define("LAQUO",  "\xAB");
	define("RAQUO",  "\xBB");

	define("LDQUO",  "\x84");
	define("RDQUO",  "\x93");

	define("LDQUO1",  "\x91");
	define("RDQUO1",  "\x92");

	define("RDQUO2", "\x94");
	define("MDASH",  "\x97");
	define("NDASH",  "\x96");
	define("APOS",   "\xB4");
	define("HELLIP", "\x85");
	define("NUMBER", "\x98");
	define("TM",     "\x99");
	define("REG",    "\xAE");
	define("BULL",   "\x95");
}

if (DEFAULT_CHARSET === CHARSET_UTF8) {
	define("TAG1",   "\xC2\xAC"); // NOT SIGN
	define("TAG2",   "\xC2\xAD"); // SOFT HYPHEN
	define("LAQUO",  "\xC2\xAB"); // LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
	define("RAQUO",  "\xC2\xBB"); // RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK

	define("LDQUO",  "\xE2\x80\x9E"); // DOUBLE LOW-9 QUOTATION MARK
	define("RDQUO",  "\xE2\x80\x9C"); // RIGHT DOUBLE QUOTATION MARK

	define("LDQUO1", "\xE2\x80\x98"); // SINGLE TURNED COMMA QUOTATION MARK
	define("RDQUO1", "\xE2\x80\x99"); // RIGHT SINGLE QUOTATION MARK

	define("RDQUO2", "\xE2\x80\x9C"); // LEFT DOUBLE QUOTATION MARK
	define("MDASH",  "\xE2\x80\x94"); // EM DASH
	define("NDASH",  "\xE2\x80\x93"); // EN DASH
	define("APOS",   "\xD2\x91");     // CYRILLIC SMALL LETTER GHE WITH UPTURN
	define("HELLIP", "\xE2\x80\xA6"); // HORIZONTAL ELLIPSIS
	define("NUMBER", "\xE2\x84\x96"); // NUMERO SIGN
	define("TM",     "\xE2\x84\xA2"); // TRADE MARK SIGN
	define("REG",    "\xC2\xAE");     // REGISTERED SIGN
	define("BULL",   "\xE2\x80\xA2"); // BULLET
}

// функция-заменялка для тегов

$Refs = array(); // буфер для хранения тегов
$RefsCntr = 0;   // счётчик буфера

function yyyTypo($x) {
	global $Refs, $RefsCntr;
	$Refs[] = StripSlashes($x[0]);
	return TAG1.($RefsCntr++).TAG2;
}
function zzzTypo($x) {
	global $Refs;
	return $Refs[$x[1]];
}

function TypoAll($text, $isHTML = true) {
	global $Refs,$RefsCntr;
	if ($isHTML) {
		$Refs = array(); // сбрасываем буфер
		$RefsCntr = 0;   // счётчик буфера
		/*
			Вырезаем элементы, содержимое которых мы не должны преобразовывать:
			комментарии, скрипты, стили, ещё что-нибудь - по вкусу.
			Вырезанные блоки складируем в Refs при помощи функции xxxTypo()
		*/

		// комментарии
		$text = preg_replace_callback('{<!--.*?-->}s'.REGEX_MOD, 'yyyTypo', $text);

		$PrivateTags = "title|script|style|pre|textarea";
		$text = preg_replace_callback('{<\s*('.$PrivateTags.')[\s>].*?<\s*/\s*\1\s*>}is'.REGEX_MOD, 'yyyTypo', $text);

		// обычные теги
		$text = preg_replace_callback('{<(?:[^\'"\>]+|".*?"|\'.*?\')+>}s'.REGEX_MOD,'yyyTypo',$text);
	}


	// ОК. Теперь займёмся кавычками
	/*
		ВЕЛИКОЕ ПРАВИЛО: Кавычки всегда прилегают к словам!

		Открывающиеся кавычки
		могут встречаться:
			в начале строки,
			после скобок "([{",
			дефиса
			пробелов,
			ещё одной кавычки
	*/
	$prequote = '\s\(\[\{";-';
	$text = preg_replace('{^"}'.REGEX_MOD, LAQUO, $text);
	$text = preg_replace('{(?<=['.$prequote.'])"}'.REGEX_MOD, LAQUO, $text);

	// а это для тех, кто нарушает ВЕЛИКОЕ ПРАВИЛО
	$text = preg_replace('{^((?:'.TAG1.'\d+'.TAG2.')+)"}'.REGEX_MOD, '\1'.LAQUO, $text);
	$text = preg_replace('{(?<=['.$prequote.'])((?:'.TAG1.'\d+'.TAG2.')+)"}'.REGEX_MOD, '\1'.LAQUO, $text);

	/*
		Закрывающиеся кавычки - все остальные
		Вы спросите - а как же тогда ставить знак дюйма? Очень просто - &quot;!
	*/
	$text = str_replace('"', RAQUO, $text);

	// исправляем ошибки в расстановке кавычек типа ""... и ...""
	// (предполагаем, что не более двух-трёх кавык подряд)
	$text = preg_replace('{'.LAQUO.RAQUO.'}'.REGEX_MOD, LAQUO.LAQUO, $text);
	$text = preg_replace('{'.RAQUO.LAQUO.'}'.REGEX_MOD, RAQUO.RAQUO, $text);

	//    вложенные кавыки
	$i=0; // - это защита от зацикливания (оно возможно в случае неправильно расставленных кавычек)
	while (($i++<10) && preg_match('{'.LAQUO.'(?:[^'.RAQUO.']*?)'.LAQUO.'}'.REGEX_MOD, $text))
		$text = preg_replace('{'.LAQUO.'([^'.RAQUO.']*?)'.LAQUO.'(.*?)'.RAQUO.'}s'.REGEX_MOD, LAQUO.'\1'.LDQUO.'\2'.RDQUO, $text);

	$i=0;
	while (($i++<10) && preg_match('{'.RAQUO.'(?:[^'.LAQUO.']*?)'.RAQUO.'}'.REGEX_MOD, $text))
		$text = preg_replace('{'.RAQUO.'([^'.LAQUO.']*?)'.RAQUO.'}'.REGEX_MOD, RDQUO.'\1'.RAQUO, $text);

	// с кавычками закончили, займёмся мелкой типографикой
	// тире:
	$text = preg_replace('{^-+(?=\s)}'.REGEX_MOD, MDASH, $text);
	$text = preg_replace('{(?<=[\s'.TAG2.'])-+(?=\s)}'.REGEX_MOD, MDASH, $text);
	$text = str_replace(' '.MDASH, '&nbsp;'.MDASH, $text);
	// ndash:
	$text = preg_replace('{(?<=\d)-(?=\d)}'.REGEX_MOD, NDASH, $text);
	// ...:
	$text = str_replace('...', HELLIP, $text);
	// апостроф:
	$text = preg_replace('{(?<=\S)\'}'.REGEX_MOD, APOS, $text);


	if ($isHTML) {
		// возвращаем взятое обратно
		while (preg_match('{'.TAG1.'.+?'.TAG2.'}'.REGEX_MOD, $text))
			$text = preg_replace_callback('{'.TAG1.'(.+?)'.TAG2.'}'.REGEX_MOD, 'zzzTypo', $text);
	}

	// заменяем коды символов на HTML-entities.
	if (DEFAULT_CHARSET !== CHARSET_UTF8) {
		$text = str_replace(
			array(LAQUO,RAQUO,LDQUO,RDQUO,RDQUO2,MDASH,NDASH,HELLIP,APOS, NUMBER, LDQUO1,RDQUO1, TM, REG, BULL),
			array('&laquo;','&raquo;','&bdquo;','&ldquo;','&rdquo;','&#8212;','&#8211;','&hellip;','&#8217;', '&#8470;','&ldquo;','&rdquo;', '&trade;', '&reg;', '&bull;'),
			$text
		);
	}
	return $text;
}

function TypoAllRecursive($mixed, $isHTML = true) {
	if (is_array($mixed)) {
		foreach ($mixed as $k=>$v) {
			$mixed[$k] = TypoAllRecursive($v, $isHTML);
		}
	} else {
		$mixed = TypoAll($mixed, $isHTML);
	}
	return $mixed;
}


function UnTypoAll($text) {
	$text = str_replace(
		array('&laquo;','&raquo;','&bdquo;','&ldquo;','&rdquo;','&#8212;','&#8211;','&hellip;','&#8217;', '&#8470;','&ldquo;','&rdquo;'),
		array('"', '"', '"', '"', '"', '-', '-', '...', "'", NUMBER, '"', '"'),
		$text
	);

	return $text;
}

function UnTypoAllRecursive($mixed) {
	if (is_array($mixed)) {
		foreach ($mixed as $k=>$v) {
			$mixed[$k] = UnTypoAllRecursive($v);
		}
	} else {
		$mixed = UnTypoAll($mixed);
	}
	return $mixed;
}
?>
