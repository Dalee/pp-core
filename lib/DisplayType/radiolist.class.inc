<?php
class PXDisplayTypeRadioList extends PXDisplayTypeDropDown {
	public function buildInput($field, $object, $param = null) {
		$value = !empty($object[$field->name]) ? $object[$field->name] : 0;
		$values  = $field->values->getList();
		if(!(is_countable($values) ? count($values) : 0)){
			return;
		}

		$args = !empty($field->displayTypeArgs) ? array_flip($field->displayTypeArgs) : null;

		if (isset($args['null'])){
			$values = [null => "---"] + $values;
		}

		if (isset($args['notnull'])){
			unset($values['']);
		}

		$selector = new PXInputRadiolist($field->name, $value, $values);
		return $selector->html();
	}
}
