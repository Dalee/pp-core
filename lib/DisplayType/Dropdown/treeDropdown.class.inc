<?php
/**
 * Выпадающий список с деревом объектов из связанного справочника(directory) указанного в аттрибуте source
 *
 * storageType integer
 * displayType TREEDROPDOWN|width
 *
 */

class PXDisplayTypeTreedropdown extends PXDisplayTypeDropdown
{
	public function buildInput($field, $object, $param = NULL)
	{
		$value = $object[$field->name] ?? NULL;

		return NLAbstractHTMLForm::BuildDropDown($field->name, $value, $this->getListFromField($field));
	}

	protected function getListFromField($field)
	{
		$tree = $field->values->GetTree();
		return $tree ? $tree->GetPlainTree(0) : [];
	}
}
