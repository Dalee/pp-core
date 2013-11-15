<?php
	class RELS_Regex_Routine extends RELS_Common_Routine {
		var $_mapData;
		var $_assertData  = array();
		var $__exceptions = array(
			'NOMAPDATAGIVEN'         => 'При вызове парсера RELS не передан список значений для сравнения!',
			'MAPDATADIFFER'          => 'Список переданных значений отличается от списка разрешенных полей!',
			'NUMERICEXPECTED'        => 'Для поля "%s" ожидалось числовое значение, но было передано значение типа "%s"',
			'REGEXPASSERTIONFAILED'  => 'Ошибка %s: регулярное выражение "%s" для поля "%s" содержит синтаксические ошибки PCRE.',
			'RESTRICTEDTYPES'        => 'Значение поля "%s" содержит запрещенный для сравнения тип "%s"'
		);
		
		function RELS_Regex_Routine($allowedFields, $fieldsData){
			parent::RELS_Common_Routine($allowedFields);
			if(!is_array($fieldsData) || !sizeof($fieldsData)){
				$this->throwNoMapDataGiven();
				FatalError($this->getLastError());
			}
			if(sizeof(array_diff($allowedFields, array_keys($fieldsData)))){
				$this->throwMapDataDiffer();
				FatalError($this->getLastError());
			}
			$this->_mapData = $fieldsData;
			//activate assertion for testing purposes
			assert_options(ASSERT_ACTIVE, 1);
			assert_options(ASSERT_WARNING, 0);
			assert_options(ASSERT_QUIET_EVAL, 1);
		}
		
		function __op_eq($rule){
			$this->_simpleParser($rule, true);
		}
		
		function __op_ne($rule){
			$this->_simpleParser($rule, false);
		}
		
		function __op_similar($rule){
			$this->_complexParser($rule, true);
		}
		
		function __op_not_similar($rule){
			$this->_complexParser($rule, false);
		}
		
		function _simpleParser($rule, $positive){
			//TODO: implement boolean comparsion
			if($this->_restrictedTypes($this->_mapData[$rule[1]])){
				$this->throwRestrictedTypes($rule[1],gettype($this->_mapData[$rule[1]]));
				return;
			}
			if(isset($rule[4])){
			#################### COMPLEX EXPRESSION ########################
				$rule[3] = substr($rule[3], 1, -1);
				//parse simple rules for PCRE syntax compatibility
				$rule[3] = $this->_parseRegular($rule[3]);
				$exp = ($positive ? '' : '!') . 'preg_match("#' . $rule[3] . '#i", "' . $this->_mapData[$rule[1]] . '")';
			} else { 
			#################### SIMPLE NUMERIC EXPRESSION #################
				if(is_numeric($this->_mapData[$rule[1]])){
					if(strpos($rule[3], ',') !== false){
						$exp = ($positive ? '' : '!') . 'in_array(' . $this->_mapData[$rule[1]] . ', array('. $rule[3] .'))';
					} else {
						$exp = $this->_mapData[$rule[1]] . ($positive ? ' == ' : ' != ') .  $rule[3];
					}
				} elseif($this->_mapData[$rule[1]] == ""){
					$exp = ($positive ? '' : '!') . "false";
				} else {
					return $this->throwNumericExpected($rule[1], gettype($this->_mapData[$rule[1]]));
				}
			}
			
			$this->_res = str_lreplace($rule[0], $exp, $this->_res);
		}
		
		function _complexParser($rule, $positive){
			if($this->_restrictedTypes($this->_mapData[$rule[1]])){
				$this->throwRestrictedTypes($rule[1],gettype($this->_mapData[$rule[1]]));
				return;
			}
			if(isset($rule[4])){
				$rule[3] = substr($rule[3], 1, -1);
				$exp = ($positive ? '' : '!') . 'preg_match("' . $rule[3] . '", "' . $this->_mapData[$rule[1]] . '")';
				if(!assert("preg_match('$rule[3]', 'assert') !== false")){
					return $this->throwRegexpAssertionFailed(strtoupper(get_class($this)), $rule[3], $rule[1]);
				}
			} else {
				return $this->throwRegexpCannotBeNumeric(strtoupper(get_class($this)), $rule[2]);
			}
			$this->_res = str_lreplace($rule[0], $exp, $this->_res);
		}
		
		function _restrictedTypes($var){
			return is_array($var) || is_object($var);
		}
		
		function _parseRegular($string){
			//save quoted *
			$string = str_replace('\\*', chr(1), $string);
			//save unquoted *
			$string = str_replace('*',   chr(2), $string);
			//save quoted _
			$string = str_replace('\\_', chr(3), $string);
			//save unquoted _
			$string = str_replace('_',   chr(4), $string);
			//apply simple expression logic, ex. : ^search string$
			list($string, $modifiers) = $this->_parseAnkors($string);
			
			//make preg quote for the rest of symbols
			$string = preg_quote($string, "#");
			
			//unquoted _ becomes to .
			$string = str_replace(chr(4), '.', $string);
			//restore quoted _
			$string = str_replace(chr(3), '_', $string);
			//unquoted * becomes to .+
			$string = str_replace(chr(2), '.+', $string);
			//restore quoted *
			$string = str_replace(chr(1), '\\*', $string);
			//restore ^ $
			if(!($this->searchFromLeft & $modifiers)){
				$string = '^' . $string;
			}
			if(!($this->searchToRight & $modifiers)){
				$string = $string . '$';
			}
			
			return $string;
		}
		
	}
?>
