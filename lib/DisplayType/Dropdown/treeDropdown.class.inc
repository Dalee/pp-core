<?php
/**
 * ���������� ������ � ������� �������� �� ���������� �����������(directory) ���������� � ��������� source 
 *
 * storageType integet
 * displayType TREEDROPDOWN|width
 *             width �� ������������
 * 
 */

class PXDisplayTypeTreedropdown extends PXDisplayTypeDropdown {
	function buildInput($field, $object, $param=NULL) {
		$value = isset($object[$field->name]) ? htmlspecialchars($object[$field->name]) : NULL;
		$referencedTree =  $field->values->GetTree();
	
		return NLAbstractHTMLForm::BuildDropDown($field->name, $value, $referencedTree->getPlainTree(0));
	}
}
?>