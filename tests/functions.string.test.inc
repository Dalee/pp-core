<?php
/**
 * Testing lib/Common/functions.string.inc
 * $ sudo -u nobody ./libpp/sbin/pp.php test:all
 *
 */
// Base string manupulation routines
t(
	'_StrToLower',
	function () { return _StrToLower(myconv(CHARSET_UTF8, DEFAULT_CHARSET, 'ПРИВЕТ')); },
	function ($result) { return (strcmp($result, myconv(CHARSET_UTF8, DEFAULT_CHARSET, 'привет')) == 0); }
);
t(
	'_StrToUpper',
	function () { return _StrToUpper(myconv(CHARSET_UTF8, DEFAULT_CHARSET, 'привет')); },
	function ($result) { return (strcmp($result, myconv(CHARSET_UTF8, DEFAULT_CHARSET, 'ПРИВЕТ')) == 0); }
);
t(
	'_Translit',
	function () { return _Translit(myconv(CHARSET_UTF8, DEFAULT_CHARSET, 'привет')); },
	function ($result) { return (strcmp($result, 'privet') == 0); }	
);
t(
	'_TranslitFilename',
	function () { return _TranslitFilename(myconv(CHARSET_UTF8, DEFAULT_CHARSET, '/полный/путь/проксима')); },
	function ($result) { return (strcmp($result, '_polnij_put_proksima') == 0); }
);
t(
	'_TranslitUnique',
	function () { return _TranslitUnique(myconv(CHARSET_UTF8, DEFAULT_CHARSET, 'это просто строка')); },
	function ($result) { return (strcmp($result, 'jeto prosto stroka') == 0); }
);

// Helpers
t(
	'NumericEndingsRussian',
	function () {
		$one  = myconv(CHARSET_UTF8, DEFAULT_CHARSET, 'документ');
		$two  = myconv(CHARSET_UTF8, DEFAULT_CHARSET, 'документа');
		$zero = myconv(CHARSET_UTF8, DEFAULT_CHARSET, 'документов');
		return array(
			'1' => NumericEndingsRussian(1, $one, $two, $zero),
			'2' => NumericEndingsRussian(2, $one, $two, $zero),
			'5' => NumericEndingsRussian(5, $one, $two, $zero),
		);
	},
	function ($result) {
		$one  = myconv(CHARSET_UTF8, DEFAULT_CHARSET, 'документ');
		$two  = myconv(CHARSET_UTF8, DEFAULT_CHARSET, 'документа');
		$zero = myconv(CHARSET_UTF8, DEFAULT_CHARSET, 'документов');
		return 
			(strcmp($result['1'], $one) == 0) &&
			(strcmp($result['2'], $two) == 0) &&
			(strcmp($result['5'], $zero) == 0);
	}
);

// URL manipulation
t(
	'appendParamToUrl',
	function () {
		return array(
			'1' => appendParamToUrl('/hello/world', 'hello', 'world'),
//			'2' => appendParamToUrl(myconv(CHARSET_UTF8, DEFAULT_CHARSET, '/привет/мир'), 'hello', myconv(CHARSET_UTF8, DEFAULT_CHARSET, 'привет'))
		);
	},
	function ($result) {
		return 
			(strcmp($result['1'], 'http:///hello/world?hello=world') == 0) &&
//			(strcmp($result['2'], myconv(CHARSET_UTF8, DEFAULT_CHARSET, 'http:///привет/мир?hello=%D0%D2%C9%D7%C5%D4')) == 0);
			1;
	}
);
t(
	'removeParamFromUrl',
	function () {
		return array(
			'1' => removeParamFromUrl('/hello/world?hello=world', 'hello'),
			'2' => removeParamFromUrl(myconv(CHARSET_UTF8, DEFAULT_CHARSET, '/привет/мир?hello=%D0%D2%C9%D7%C5%D4'), 'hello')
		);
	},
	function ($result) {
		return 
			(strcmp($result['1'], 'http:///hello/world') == 0) &&
			(strcmp($result['2'], myconv(CHARSET_UTF8, DEFAULT_CHARSET, 'http:///привет/мир')) == 0);
	}
);
t(
	'isUtfString and isAsciiString',
	function () {
		return array(
			'1' => isUtfString('привет'),
			'2' => isAsciiString('hello')
		);
	},
	function ($result) {
		return ($result['1'] === true) && ($result['2'] === true);
	}
);
