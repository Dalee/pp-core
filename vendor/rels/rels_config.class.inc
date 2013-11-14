<?php
	class RELS_Config {

		var $comparsionOperands = array(
			'='   => '__op_eq',
			'=~'  => '__op_similar',
			'!='  => '__op_ne',
			'!=~' => '__op_not_similar',
			'<'   => '__op_lt',
			'<='  => '__op_le',
			'>'   => '__op_gt',
			'>='  => '__op_ge'
		);
		
		var $logicOperands = array(
			'|' => '__op_or',
			'&' => '__op_and'
		);

		var $exceptions = array(
			'UNKNOWNEXCEPTION'       => 'Неизвестная ошибка',
			
			'REQMUSTBESTRING'        => 'Переданный запрос не является строкой',
			'EMPTYREQ'               => 'Пустой запрос',
			'NOFIELDSGIVEN'          => 'При вызове парсера RELS не передан список разрешенных переменных!',
			
			'NOKNOWNFILEDSGIVEN'     => 'В запросе отсутствуют разрешенные переменные',
			
			'COMMONSYNTAXERROR'      => 'Ошибка синтаксиса: недопустимая переменная, оператор или выражение; позиция - %d, около символа "%s"',
			'BRACKETSYNTAXERROR'     => 'Ошибка синтаксиса: в позиции %d обнаружена недопустимая скобка',
			'LOGICSYNTAXERROR'       => 'Ошибка синтаксиса: в позиции %d обнаружен недопустимый логический символ',
			'ATENDSYNTAXERROR'       => 'Ошибка синтаксиса: в запросе присутствует незакрытая скобка или лишний логический оператор',
			
			'UNSUPPORTEDOPERAND'     => 'Ошибка %s: данный адаптер парсера RELS не поддерживает оператор %s',
			'REGEXPCANNOTBENUMERIC'  => 'Ошибка %s: оператор "%s" поддерживает только регулярные выражения, для простых числовых выражений необходимо пользоваться простыми операторами сравнения'
		);
		
		var $testingFSMTable = array(
			"begin" =>
				array(
					"openbracket"  => true,
					"closebracket" => false,
					"expression"   => true,
					"logic"        => false,
					"end"          => false
				),
			"openbracket" =>
				array(
					"openbracket"  => true,
					"closebracket" => false,
					"expression"   => true,
					"logic"        => false,
					"end"          => false
				),
			"closebracket" =>
				array(
					"openbracket"  => false,
					"closebracket" => true,
					"expression"   => false,
					"logic"        => true,
					"end"          => true
				),
			"expression" =>
				array(
					"openbracket"  => false,
					"closebracket" => true,
					"expression"   => false,
					"logic"        => true,
					"end"          => true
				),
			"logic" =>
				array(
					"openbracket"  => true,
					"closebracket" => false,
					"expression"   => true,
					"logic"        => false,
					"end"          => false
				)
		);
	
	}
?>
