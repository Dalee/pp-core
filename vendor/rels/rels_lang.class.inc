<?php
	/**
	 * @require package libpp=>functions.string.inc
	 */
	
	class RELS_Lang extends RELS_Config {
		var $_req;
		var $_res;
		var $_allowedFields;
		var $_rules  = array();
		var $_errors = array();
		
		/*protected*/ function RELS_Lang($allowedFields = array()){
			overload(get_class($this));
			if(isset($this->__exceptions) && is_array($this->__exceptions)){ //apply custom routine exceptions
				$this->exceptions = array_merge($this->exceptions, $this->__exceptions);
			}
			
			if(!is_array($allowedFields) || !sizeof($allowedFields)){
				$this->throwNoFieldsGiven();
				FatalError($this->getLastError());
			}
			
			$this->_allowedFields = $allowedFields;
		}

		function parse($req){
			if(!is_string($req)) {
				$this->throwReqMustBeString();
				return false;
			}
			$this->_req = $req;
			$this->_normalizeReq();
			if(!strlen($this->_req)) {
				$this->throwEmptyReq();
				return false;
			}
			
			return $this->_getAvailableRulesFromReq() && $this->_parseAvailableRules();
		}
		
		function getResult(){
			return $this->_res;
		}

		function _normalizeReq(){
			$this->_req = Strip1251($this->_req);
			$this->_req = str_replace(array("\t","\n","\r"), ' ', $this->_req);
			$this->_req = squeeze($this->_req, ' ');
			$this->_req = trim($this->_req);
		}
		
		function _encodeQS($string){
			return str_replace('\\\\', chr(1), $string);
		}
		
		function _restoreQS($string){
			return str_replace(chr(1), '\\', $string); //restore unquoted slashes, as is
		}
		
		function applyPregQuotes($string){
			return preg_quote($string, '#');
		}
		
		function _getAvailableRulesFromReq(){
			$this->_req = $this->_encodeQS($this->_req); //save quoted slashes as single special char, required for parser
			$_  = '(';
			$_ .= join('|',array_map(array($this,'applyPregQuotes'), $this->_allowedFields));
			$_ .= ')';
			$_ .= '\s?(';
			$_ .= join('|',array_map(array($this,'applyPregQuotes'), array_keys($this->comparsionOperands)));
			$_ .= ')\s?((?>([\'"]).*?(?<!\\\)\\4)|(?>\-?\d+(?:\s?,\s?(?=\-?\d))?)+)';
			
			if(preg_match_all("#{$_}#",$this->_req, $matches, PREG_SET_ORDER) > 0){
				$this->_req = $this->_restoreQS($this->_req);
				foreach($matches as $k => $match){
					$matches[$k] = array_map(array(&$this, '_restoreQS'), $match);
				}
				
				$this->_rules = $matches;
				return $this->_reqSyntaxCheck();
			} else {
				$this->throwNoKnownFiledsGiven();
				return false;
			}
		}

		function _reqSyntaxCheck(){
			$test_req = $this->_req;
			foreach($this->_rules as $id => $rule){
				$lot_of_spaces = str_repeat(' ', strlen($rule[0]) - 1);
				$test_req = str_lreplace($rule[0], chr(1)."{$lot_of_spaces}", $test_req);
			}

			for($i = 0; $i <= strlen($test_req); $i++){
				$chr = isset($test_req{$i}) ? $test_req{$i} : '';
				if(!$this->_processFSM($chr)){
					switch(true){
						case $chr == '(' || $chr == ')':
							$this->throwBracketSyntaxError($i);
							break;
						case in_array($chr, array_keys($this->logicOperands)):
							$this->throwLogicSyntaxError($i);
							break;
						case $chr === '':
							$this->throwAtEndSyntaxError();
							break;
						default:
							$this->throwCommonSyntaxError($i, $this->_req{$i});
							break;
					}
					return false;
				}
			}
			return true;
		}
		
		function _processFSM($chr = null){
			static $state = "begin", $b_stack = array();
			$secondary_check = true;
			if($state == 'end'){ //rewind static state to it's initial condition
				$state = 'begin';
			}

			switch(true){
				case $chr == ' ':
					return true;
				case $chr == '(':
					array_push($b_stack, true);
					$new_state = "openbracket";
					break;
				case $chr == ')':
					$new_state = "closebracket";
					$secondary_check = (bool)array_pop($b_stack);
					break;
				case $chr == chr(1):
					$new_state = "expression";
					break;
				case in_array($chr, array_keys($this->logicOperands)):
					$new_state = "logic";
					break;
				case $chr === '':
					$new_state = "end";
					$secondary_check = !(bool)array_pop($b_stack);
					break;
				default:
					return false;
			}
			return $this->testingFSMTable[$state][$new_state] && $secondary_check && ($state = $new_state);
		}
		
		function _parseAvailableRules(){
			$this->_res = $this->_req;
			
			foreach($this->_rules as $k => $rule){ //replace expressions with special markers before apply logic operands
				$this->_res          = str_lreplace($this->_rules[$k][0], chr(1)."{$k}", $this->_res);
				$this->_rules[$k][0] = chr(1)."{$k}";
			}
			
			foreach($this->logicOperands as $sym => $callback){ //apply logic operands
				$this->$callback($sym);
				if(sizeof($this->_errors)){
					return false;
				}
			}
			
			foreach($this->_rules as $_rule){
				#[0] - rule number
				#[1] - variable
				#[2] - operand
				#[3] - expression
				#[4] - used as flag, defines, has expression the numeric comma separated values or more complex string pattern
				
				$this->{$this->comparsionOperands[$_rule[2]]}($_rule);
				if(sizeof($this->_errors)){
					return false;
				}
			}
			$this->_res = squeeze($this->_res, ' ');
			return true;
		}

		function _RELS_Exception($type, $extraArgs){
			$errorToString = isset($this->exceptions[strtoupper($type)]) ? $this->exceptions[strtoupper($type)] : $this->exceptions['UNKNOWNEXCEPTION'];
			if(sizeof($extraArgs)){
				array_unshift($extraArgs, $errorToString);
				$errorToString = call_user_func_array("sprintf", $extraArgs);
			}
			array_push($this->_errors, $errorToString);
		}
		
		function getLastError(){
			return array_pop($this->_errors);
		}
		
		function getRulesCount(){
			return count($this->_rules);
		}
		
		function __call($meth, $args){
			switch(substr($meth, 0, 5)){
				case 'throw':
					$this->_RELS_Exception(substr($meth, 5), $args);
					break;
				case '__op_':
					$this->_RELS_Exception('UnsupportedOperand', array(strtoupper(get_class($this)), $args[0][2]));
					break;
			}
			return true; //fuck PHP4
		}



	}
?>
