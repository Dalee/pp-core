<?php
/**
 * Testing lib/Common/functions.string.inc
 * $ sudo -u nobody ./libpp/sbin/pp.php test:all
 *
 */
// Base string manupulation routines
t(
	'_StrToLower',
	function () { return _StrToLower('ПРИВЕТ'); },
	function ($result) { return (strcmp($result, 'привет') == 0); }
);
t(
	'_StrToUpper',
	function () { return _StrToUpper('привет'); },
	function ($result) { return (strcmp($result, 'ПРИВЕТ') == 0); }
);
t(
	'_Translit',
	function () { return _Translit('привет'); },
	function ($result) { return (strcmp($result, 'privet') == 0); }
);
t(
	'_TranslitFilename',
	function () { return _TranslitFilename('/полный/путь/проксима'); },
	function ($result) { return (strcmp($result, '_polnij_put_proksima') == 0); }
);
t(
	'_TranslitUnique',
	function () { return _TranslitUnique('это просто строка'); },
	function ($result) { return (strcmp($result, 'jeto prosto stroka') == 0); }
);

// Helpers
t(
	'NumericEndingsRussian',
	function () {
		$one  = 'документ';
		$two  = 'документа';
		$zero = 'документов';
		return array(
			'1' => NumericEndingsRussian(1, $one, $two, $zero),
			'2' => NumericEndingsRussian(2, $one, $two, $zero),
			'5' => NumericEndingsRussian(5, $one, $two, $zero),
		);
	},
	function ($result) {
		$one  = 'документ';
		$two  = 'документа';
		$zero = 'документов';
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
			'2' => appendParamToUrl('/привет/мир', 'hello', 'привет')
		);
	},
	function ($result) {
		return
			(strcmp($result['1'], 'http:///hello/world?hello=world') == 0) &&
			(strcmp($result['2'], 'http:///привет/мир?hello=%D0%D2%C9%D7%C5%D4') == 0);
	}
);
t(
	'removeParamFromUrl',
	function () {
		return array(
			'1' => removeParamFromUrl('/hello/world?hello=world', 'hello'),
			'2' => removeParamFromUrl('/привет/мир?hello=%D0%D2%C9%D7%C5%D4', 'hello')
		);
	},
	function ($result) {
		return
			(strcmp($result['1'], 'http:///hello/world') == 0) &&
			(strcmp($result['2'], 'http:///привет/мир') == 0);
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
