<?php

class PXDisplayTypeMultiDropDown extends PXDisplayTypeDropdown {
	function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) && is_array($object[$field->name])
			? array_map("htmlspecialchars", $object[$field->name]) 
			: NULL;

		$size = isset($field->displayTypeArgs[0])
			? $field->displayTypeArgs[0]
			: NULL;

		$value = array_key_exists(0, (array)$value) && $value[0] == "NULL" ? "" : $value;

		return NLAbstractHTMLForm::BuildDropDown($field->name . "[]", $value, $field->values->GetList(), true, $size);
	}

	// remove 0 -> NULL, 0 -> '', 0 -> "NULL" values
	function _filterNULL($value) {
		return !( (is_null($value)) || (empty($value)) || ($value == "NULL"));
	}

	function buildCell($field, $object, $param=NULL) {
		$displayField = $field->values->displayField;
		$value = array_filter($object[$field->name], array($this, "_filterNULL"));

		$values = $field->values->values;
		$values = array_intersect_key($values, array_flip($value));

		return !empty($values) ? join(", ", GetColFromTable($values, $displayField)) : '';
	}
}

?>
