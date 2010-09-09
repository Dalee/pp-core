<?php
class PXDisplayTypeMultiRegions_Static extends PXDisplayTypeMultiRegions {
	function buildInput($field, $object, $param=NULL) {
		$html = '';
		foreach($object[$field->name] as $r) {
			$html .= NLAbstractHTMLForm::BuildHidden($field->name.'[]', $r);
		}

		return $html.$this->buildCell($field, $object, $param);
	}
}
?>
