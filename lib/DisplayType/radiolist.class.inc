<?php
class PXDisplayTypeRadioList extends PXDisplayTypeDropDown {
	public function buildInput($field, $object, $param = null) {
		$value = !empty($object[$field->name]) ? $object[$field->name] : 0;
		$values  = $field->values->getList();
		if(!count($values)){
			return;
		}

		$args = !empty($field->displayTypeArgs) ? array_flip($field->displayTypeArgs) : null;

		if (isset($args['null']))
			$values = array(null => "---") + $values;

		$selector = new PXInputRadiolist($field->name, $value, $values);
		return $selector->html();
	}
}
?>