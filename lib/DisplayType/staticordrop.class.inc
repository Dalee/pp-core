<?php
class PXDisplayTypeStaticOrDrop extends PXDisplayType {
	var $ready        = 0;
	var $process      = 10;
	var $process_test = 11;
	var $send         = 20;
	var $send_test    = 21;
	
	function buildInput($field, $object, $param = NULL) {
		$value   = isset($object[$field->name]) ? $object[$field->name] : NULL;
		$values = isset($field->values) ? $field->values->GetList() : array();
		$fvalues = $field->values->values;
		
		if (!empty($fvalues[$value]['static'])) {
			return NLAbstractHTMLForm::BuildHidden($field->name, $value) . htmlspecialchars(isset($values[$value]) ? $values[$value] : $value, ENT_COMPAT | ENT_HTML401, DEFAULT_CHARSET);
		} else {
			foreach ($fvalues as $k => $fval) {
				if (isset($fval['static'])) {
					unset($values[$fval['id']]);
				}
			}
			return NLAbstractHTMLForm::BuildDropDown($field->name, $value, $values);
		}
	}
	
	function buildCell($field, $object, $param = NULL) {
		$value = $object[$field->name];
		$dropdown = new PXDisplayTypeDropdown($this->display, $this->name);
		return $dropdown->buildCell($field, $object, $param);
	}
}
