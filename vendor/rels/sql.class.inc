<?php
	class RELS_PGSQL_Routine extends RELS_Common_Routine {
		var $_subReqData;
		var $_types;
		var $_regexpMode = false;
		var $__exceptions = array(
			'TYPEMISMATCH'       => 'Ошибка типов: для поля "%s" ожидалось значение типа "%s"',
			'UNSUPPORTEDTYPE'    => 'Ошибка %s: оператор "%s" не поддерживает сравнение с типом "%s"',
			'COMPLEXUNSUPPORTED' => 'Ошибка %s: оператор "%s" не может использоваться для поиска по образцу'
		);
		
		function RELS_PGSQL_Routine($allowedFields, $subReqArgs = array('self' => 'id', 'parent' => 'parent')){
			$allowedFields = $this->_applyTypes($allowedFields);
			parent::RELS_Common_Routine($allowedFields);
			$this->_subReqData = $subReqArgs;
		}
		
		function _applyTypes($fields){
			$myTypes = RELS_PGSQL_Routine::allowedTypes();
			foreach($fields as $ind => $field){
				foreach($myTypes as $type){
					if(($pos = strpos($field, "({$type})")) > 0){
						$fields[$ind]                = substr($field, 0, $pos);
						$this->_types[$fields[$ind]] = $type;
					}
				}
			}
			return $fields;
		}
		
		//static method, return allowed types
		function allowedTypes(){
			return array('numeric', 'boolean');
		}
		
		function __op_eq($rule){
			$this->_sql_parser($rule, array('__common' => " LIKE '%s' ", 'numeric' => " = '%s' ", 'boolean' => " = '%s' ", '__numeric_list' => ' IN (%s) '));
		}
		
		function __op_ne($rule){
			$this->_sql_parser($rule, array('__common' => " NOT LIKE '%s' ", 'numeric' => " <> '%s' ", 'boolean' => " <> '%s' ", '__numeric_list' => ' NOT IN (%s) '));
		}
		
		function __op_similar($rule){
			$this->_sql_parser($rule, array('__regexp' => " ~ '%s' "));
		}
		
		function __op_not_similar($rule){
			$this->_sql_parser($rule, array('__regexp' => " !~ '%s' "));
		}
		
		function __op_lt($rule){
			$this->_sql_parser($rule, array('numeric' => " < '%s' "));
		}
		
		function __op_le($rule){
			$this->_sql_parser($rule, array('numeric' => " <= '%s' "));
		}
		
		function __op_gt($rule){
			$this->_sql_parser($rule, array('numeric' => " > '%s' "));
		}
		
		function __op_ge($rule){
			$this->_sql_parser($rule, array('numeric' => " >= '%s' "));
		}
		
		function _sql_parser($rule, $operators_map){
			$sql = $this->_sql_expression($rule, $operators_map);
			$this->_res = str_lreplace($rule[0], $sql, $this->_res);
		}
		
		function _sql_expression($rule, $operators_map){
			if(strpos($rule[1], '.') !== false){
				$sql = $this->_sql_subrequest($rule, $operators_map);
			} else {
				$sql = $this->_sql_comparable($rule, $operators_map);
			}
			return $sql;
		}
		
		function _sql_subrequest($rule, $operators_map){
			list($parent, $parentField) = explode('.', $rule[1], 2);
			$rule[1] = $parentField;
			$sql     = $this->_subReqData['parent'] . ' IN (SELECT '. $this->_subReqData['self'] .' FROM '.pg_escape_string($parent).' WHERE ';
			$sql    .= $this->_sql_expression($rule, $operators_map);
			$sql    .= ') ';
			return $sql;
		}
		
		function _sql_comparable($rule, $operators_map){
			if(isset($rule[4])){
			#################### COMPLEX EXPRESSION ########################
				$rule[3] = substr($rule[3], 1, -1);
				switch(true){
					case !isset($this->_types[$rule[1]]):
						$sql = " lower({$rule[1]}) ";
						if(isset($operators_map['__regexp'])){
							//CAUTION: Use this mode at your own risk !
							//build case insensitive PostgreSQL regexp string using ~ (instead of ~*), thus we can use indexes if search string is starting with ^
							$sql .= sprintf($operators_map['__regexp'], pg_escape_string(strtolower($rule[3])));
						} elseif(isset($operators_map['__common'])) {
							//apply simple expression logic, ex. : ^search string$
							list($rule[3], $modifiers) = $this->_parseAnkors($rule[3]);
							$_l = ($this->searchFromLeft & $modifiers) ? '%' : '';
							$_r = ($this->searchToRight  & $modifiers) ? '%' : '';
							//parse multipliers
							$rule[3] = $this->_parseMultipliers($rule[3]);
							//build case insensitive PostgreSQL search string using LIKE (instead of ILIKE), thus we can use indexes if search string is not starting with % or _
							$sql .= sprintf($operators_map['__common'], $_l . pg_escape_string(strtolower($rule[3])) . $_r);
						} else {
							$this->throwComplexUnsupported(strtoupper(get_class($this)), $rule[2]);
						}
					break;
					case $this->_types[$rule[1]] == 'numeric' && is_numeric($rule[3]):
						if(!isset($operators_map[$this->_types[$rule[1]]])){
							$this->throwUnsupportedType(strtoupper(get_class($this)), $rule[2], $this->_types[$rule[1]]);
							return '';
						}
						$sql = $rule[1] . sprintf($operators_map[$this->_types[$rule[1]]], $rule[3]);
					break;
					case $this->_types[$rule[1]] == 'boolean' && $this->_likeBoolean($rule[3], '?'):
						if(!isset($operators_map[$this->_types[$rule[1]]])){
							$this->throwUnsupportedType(strtoupper(get_class($this)), $rule[2], $this->_types[$rule[1]]);
							return '';
						}
						$sql = $rule[1] . sprintf($operators_map[$this->_types[$rule[1]]], (int)$this->_likeBoolean($rule[3]));
					break;
					default:
						$this->throwTypeMismatch($rule[1], $this->_types[$rule[1]]);
						return '';
				}
			} else { 
			#################### SIMPLE NUMERIC EXPRESSION #################
				switch(isset($this->_types[$rule[1]]) && $this->_types[$rule[1]] == 'boolean'){
					case true:
						if(!isset($operators_map[$this->_types[$rule[1]]])){
							$this->throwUnsupportedType(strtoupper(get_class($this)), $rule[2], $this->_types[$rule[1]]);
							return '';
						}
						if($rule[3] === '0' || $rule[3] === '1'){
							$sql = $rule[1] . sprintf($operators_map[$this->_types[$rule[1]]], $rule[3]);
						} else {
							$this->throwTypeMismatch($rule[1], $this->_types[$rule[1]]);
							return '';
						}
					break;
					case false:
						if(strpos($rule[3], ',') !== false){
							if(!isset($operators_map['__numeric_list'])){
								$this->throwUnsupportedType(strtoupper(get_class($this)), $rule[2], '__numeric_list');
								return '';
							}
							$sql = $rule[1] . sprintf($operators_map['__numeric_list'], preg_replace('#(\d+)#',"'\\1'",$rule[3]));
						} else {
							if(!isset($operators_map['numeric'])){
								$this->throwUnsupportedType(strtoupper(get_class($this)), $rule[2], 'numeric');
								return '';
							}
							$sql = $rule[1] . sprintf($operators_map['numeric'], $rule[3]);
						}
				}
			}
			return $sql;
		}
		
		function _parseMultipliers($string){
			//strip %
			$string = str_replace('%', '', $string);
			//save quoted *
			$string = str_replace('\\*', chr(1), $string);
			//replace * with %
			$string = str_replace('*', '%', $string);
			//restore previously saved quoted *
			$string = str_replace(chr(1), '*', $string);
			
			return $string;
		}
	}
?>
