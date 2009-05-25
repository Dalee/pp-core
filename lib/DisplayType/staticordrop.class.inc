<?php
class PXDisplayTypeStaticOrDrop extends PXDisplayType {
	var $ready        = 0;
	var $process      = 10;
	var $process_test = 11;
	var $send         = 20;
	var $send_test    = 21;
	
	function buildInput($field, $object, $param=NULL) {
		$value   = isset($object[$field->name]) ? htmlspecialchars($object[$field->name]) : NULL;
		$values = isset($field->values) ? $field->values->GetList() : array();
		$fvalues = $field->values->values;
		
		if (count($fvalues) 
			&& isset($fvalues[$value]) 
			&& !empty($fvalues[$value]['static'])) {
			
				return NLAbstractHTMLForm::BuildHidden($field->name, $value).(isset($values[$value]) ? $values[$value] : $value);
		} else {
				foreach ($fvalues as $k => $fval) {
					if (isset($fval['static'])) {
						$id2unset = $fval['id'];
						unset($values[$id2unset]);
					}
				} 
				return NLAbstractHTMLForm::BuildDropDown($field->name, $value, $values);
		}
	}
	
	function buildCell($field, $object, $param=NULL) {
		$value = $object[$field->name];
		return PXDisplayTypeDropdown::buildCell($field, $object, $param=NULL);
	}
}
?>