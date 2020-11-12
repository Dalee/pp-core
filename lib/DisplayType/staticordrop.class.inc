<?php

class PXDisplayTypeStaticOrDrop extends PXDisplayType
{
	public $ready = 0;
	public $process = 10;
	public $process_test = 11;
	public $send = 20;
	public $send_test = 21;

	public function buildInput($field, $object, $param = NULL)
	{
		$value = isset($object[$field->name]) ? $object[$field->name] : NULL;
		$values = isset($field->values) ? $field->values->GetList() : [];
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

	public function buildCell($field, $object, $param = NULL)
	{
		$value = $object[$field->name];
		$dropdown = new PXDisplayTypeDropdown($this->display, $this->name);
		return $dropdown->buildCell($field, $object, $param);
	}
}

