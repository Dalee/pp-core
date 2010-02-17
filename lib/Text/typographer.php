<?php
/*
Константы для удобства. Коды соответствуют символам в win-кодировке, но это
"совпадение":) - на самом деле, скрипт кросскодировочный (win и koi).
*/

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
		$text = preg_replace_callback('{<!--.*?-->}s', 'yyyTypo', $text);

		$PrivateTags = "title|script|style|pre|textarea";
		$text = preg_replace_callback('{<\s*('.$PrivateTags.')[\s>].*?<\s*/\s*\1\s*>}is', 'yyyTypo', $text);

		// обычные теги
		$text = preg_replace_callback('{<(?:[^\'"\>]+|".*?"|\'.*?\')+>}s','yyyTypo',$text);
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
	$text = preg_replace('{^"}', LAQUO, $text);
	$text = preg_replace('{(?<=['.$prequote.'])"}', LAQUO, $text);

	// а это для тех, кто нарушает ВЕЛИКОЕ ПРАВИЛО
	$text = preg_replace('{^((?:'.TAG1.'\d+'.TAG2.')+)"}', '\1'.LAQUO, $text);
	$text = preg_replace('{(?<=['.$prequote.'])((?:'.TAG1.'\d+'.TAG2.')+)"}', '\1'.LAQUO, $text);

	/*
		Закрывающиеся кавычки - все остальные
		Вы спросите - а как же тогда ставить знак дюйма? Очень просто - &quot;!
	*/
	$text = str_replace('"', RAQUO, $text);

	// исправляем ошибки в расстановке кавычек типа ""... и ...""
	// (предполагаем, что не более двух-трёх кавык подряд)
	$text = preg_replace('{'.LAQUO.RAQUO.'}', LAQUO.LAQUO, $text);
	$text = preg_replace('{'.RAQUO.LAQUO.'}', RAQUO.RAQUO, $text);

	//    вложенные кавыки
	$i=0; // - это защита от зацикливания (оно возможно в случае неправильно расставленных кавычек)
	while (($i++<10) && preg_match('{'.LAQUO.'(?:[^'.RAQUO.']*?)'.LAQUO.'}', $text))
		$text = preg_replace('{'.LAQUO.'([^'.RAQUO.']*?)'.LAQUO.'(.*?)'.RAQUO.'}s', LAQUO.'\1'.LDQUO.'\2'.RDQUO, $text);

	$i=0;
	while (($i++<10) && preg_match('{'.RAQUO.'(?:[^'.LAQUO.']*?)'.RAQUO.'}', $text))
		$text = preg_replace('{'.RAQUO.'([^'.LAQUO.']*?)'.RAQUO.'}', RDQUO.'\1'.RAQUO, $text);

	// с кавычками закончили, займёмся мелкой типографикой
	// тире:
	$text = preg_replace('{^-+(?=\s)}',MDASH,$text);
	$text = preg_replace('{(?<=[\s'.TAG2.'])-+(?=\s)}',MDASH,$text);
	$text = str_replace(' '.MDASH,'&nbsp;'.MDASH,$text);
	// ndash:
	$text = preg_replace('{(?<=\d)-(?=\d)}',NDASH,$text);
	// ...:
	$text = str_replace('...',HELLIP,$text);
	// апостроф:
	$text = preg_replace('{(?<=\S)\'}',APOS,$text);


	if ($isHTML) {
		// возвращаем взятое обратно
		while (preg_match('{'.TAG1.'.+?'.TAG2.'}', $text))
			$text = preg_replace_callback('{'.TAG1.'(.+?)'.TAG2.'}', 'zzzTypo', $text);
	}

	// заменяем коды символов на HTML-entities.
	$text = str_replace(
		array(LAQUO,RAQUO,LDQUO,RDQUO,RDQUO2,MDASH,NDASH,HELLIP,APOS, NUMBER, LDQUO1,RDQUO1, TM, REG, BULL),
		array('&laquo;','&raquo;','&bdquo;','&ldquo;','&rdquo;','&#8212;','&#8211;','&hellip;','&#8217;', '&#8470;','&ldquo;','&rdquo;', '&trade;', '&reg;', '&bull;'),
		$text
	);
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
		array('"', '"', '"', '"', '"', '-', '-', '...', "'", "\x98", '"', '"'),
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
