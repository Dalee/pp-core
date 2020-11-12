<?php

class PXDisplayTypeMultiDropDown extends PXDisplayTypeDropdown
{
	public function buildInput($field, $object, $param = NULL)
	{
		$value = isset($object[$field->name]) && is_array($object[$field->name])
			? $object[$field->name]
			: NULL;

		$size = isset($field->displayTypeArgs[0])
			? $field->displayTypeArgs[0]
			: NULL;

		$value = array_key_exists(0, (array)$value) && $value[0] == "NULL" ? "" : $value;

		return NLAbstractHTMLForm::BuildDropDown($field->name . "[]", $value, $this->getListFromField($field), true, $size);
	}

	public function _filterNULL($value)
	{
		return !is_null($value);
	}

	public function buildCell($field, $object, $param = NULL)
	{
		$displayField = $field->values->displayField;

		$value = [];
		if (is_array($object[$field->name])) {
			$value = array_filter($object[$field->name], [$this, "_filterNULL"]);
		}

		$values = $field->values->values;
		$values = array_intersect_key($values, array_flip($value));
		return !empty($values) ? join(", ", GetColFromTable($values, $displayField)) : '';
	}
}
