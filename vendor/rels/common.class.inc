<?php
	class RELS_Common_Routine extends RELS_Lang {
		var $searchFromLeft = 1;
		var $searchToRight  = 2;
		
		function RELS_Common_Routine($allowedFields){
			parent::RELS_Lang($allowedFields);
		}
		
		/*protected*/ function _parseAnkors($string){
			$modifiers = $this->searchFromLeft|$this->searchToRight;
			if(substr($string, 0, 1) == '^'){
				$modifiers &= $this->searchToRight;
				$string     = substr($string, 1);
			} elseif(substr($string, 0, 2) == '\\^'){ //remove slash before ^
				$string     = substr($string, 1);
			}
			if(substr($string, -1) == '$'){
				if(substr($string, -2, -1) != '\\'){
					$modifiers &= $this->searchFromLeft;
					$string     = substr($string, 0, -1);
				} else{//remove slash before $
					$string = substr_replace($string, '', -2, 1);
				}
			}
			
			return array($string, $modifiers);
		}
		
		function __op_or($or){
			$this->_res = str_replace($or ,' OR ',$this->_res);
		}
		
		function __op_and($and){
			$this->_res = str_replace($and ,' AND ',$this->_res);
		}
		
		function _likeBoolean($str, $behaviour = null){
			$true  = array(1, true,  '1', 'true', 'yes', 'y');
			$false = array(0, false, '0', 'false', 'no', 'n');
			return $behaviour == '?' ? in_array($str, array_merge($true, $false), true) : in_array($str, $true, true);
		}
	}
?>