<?php
/*
��������� ��� ��������. ���� ������������� �������� � win-���������, �� ���
"����������":) - �� ����� ����, ������ ����������������� (win � koi).
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

// �������-��������� ��� �����

$Refs = array(); // ����� ��� �������� �����
$RefsCntr = 0;   // �ޣ���� ������

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
		$Refs = array(); // ���������� �����
		$RefsCntr = 0;   // �ޣ���� ������
		/*
			�������� ��������, ���������� ������� �� �� ������ ���������������:
			�����������, �������, �����, �ݣ ���-������ - �� �����.
			���������� ����� ���������� � Refs ��� ������ ������� xxxTypo()
		*/

		// �����������
		$text = preg_replace_callback('{<!--.*?-->}s', 'yyyTypo', $text);

		$PrivateTags = "title|script|style|pre|textarea";
		$text = preg_replace_callback('{<\s*('.$PrivateTags.')[\s>].*?<\s*/\s*\1\s*>}is', 'yyyTypo', $text);

		// ������� ����
		$text = preg_replace_callback('{<(?:[^\'"\>]+|".*?"|\'.*?\')+>}s','yyyTypo',$text);
	}


	// ��. ������ ���ͣ��� ���������
	/*
		������� �������: ������� ������ ��������� � ������!

		������������� �������
		����� �����������:
			� ������ ������,
			����� ������ "([{",
			������
			��������,
			�ݣ ����� �������
	*/
	$prequote = '\s\(\[\{";-';
	$text = preg_replace('{^"}', LAQUO, $text);
	$text = preg_replace('{(?<=['.$prequote.'])"}', LAQUO, $text);

	// � ��� ��� ���, ��� �������� ������� �������
	$text = preg_replace('{^((?:'.TAG1.'\d+'.TAG2.')+)"}', '\1'.LAQUO, $text);
	$text = preg_replace('{(?<=['.$prequote.'])((?:'.TAG1.'\d+'.TAG2.')+)"}', '\1'.LAQUO, $text);

	/*
		������������� ������� - ��� ���������
		�� �������� - � ��� �� ����� ������� ���� �����? ����� ������ - &quot;!
	*/
	$text = str_replace('"', RAQUO, $text);

	// ���������� ������ � ����������� ������� ���� ""... � ...""
	// (������������, ��� �� ����� ����-�ң� ����� ������)
	$text = preg_replace('{'.LAQUO.RAQUO.'}', LAQUO.LAQUO, $text);
	$text = preg_replace('{'.RAQUO.LAQUO.'}', RAQUO.RAQUO, $text);

	//    ��������� ������
	$i=0; // - ��� ������ �� ������������ (��� �������� � ������ ����������� ������������� �������)
	while (($i++<10) && preg_match('{'.LAQUO.'(?:[^'.RAQUO.']*?)'.LAQUO.'}', $text))
		$text = preg_replace('{'.LAQUO.'([^'.RAQUO.']*?)'.LAQUO.'(.*?)'.RAQUO.'}s', LAQUO.'\1'.LDQUO.'\2'.RDQUO, $text);

	$i=0;
	while (($i++<10) && preg_match('{'.RAQUO.'(?:[^'.LAQUO.']*?)'.RAQUO.'}', $text))
		$text = preg_replace('{'.RAQUO.'([^'.LAQUO.']*?)'.RAQUO.'}', RDQUO.'\1'.RAQUO, $text);

	// � ��������� ���������, ���ͣ��� ������ ������������
	// ����:
	$text = preg_replace('{^-+(?=\s)}',MDASH,$text);
	$text = preg_replace('{(?<=[\s'.TAG2.'])-+(?=\s)}',MDASH,$text);
	$text = str_replace(' '.MDASH,'&nbsp;'.MDASH,$text);
	// ndash:
	$text = preg_replace('{(?<=\d)-(?=\d)}',NDASH,$text);
	// ...:
	$text = str_replace('...',HELLIP,$text);
	// ��������:
	$text = preg_replace('{(?<=\S)\'}',APOS,$text);


	if ($isHTML) {
		// ���������� ������ �������
		while (preg_match('{'.TAG1.'.+?'.TAG2.'}', $text))
			$text = preg_replace_callback('{'.TAG1.'(.+?)'.TAG2.'}', 'zzzTypo', $text);
	}

	// �������� ���� �������� �� HTML-entities.
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
