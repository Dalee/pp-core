<?php
/**
 * Выпадающий список с деревом разделов (как правило поле parent)
 *
 * storageType integer
 * displayType PARENTDROPDOWN|width
 *             width не используется
 */
class PXDisplayTypeParentdropdown extends PXDisplayTypeDropdown
{
	public function buildInput($field, $object, $param = NULL)
	{
		$value = $object[$field->name] ?? NULL;
		[$format, $parentFormat] = parent::getFormats($param);

		$tree = parent::getParentTree($object, $parentFormat);
		$restrictedId = $format == $parentFormat ? getFromArray($object, 'id') : null;
		$parents = $tree->getPlainTree($restrictedId);
		if (isset($param['root_title'])) {
			$parents[NULL] = $param['root_title'];
		}

		foreach ($parents as $k => $v) {

			if (isset($tree->leafs[$k])) {
				@$allowed =& $tree->leafs[$k]->content['allowed'];
			}

			if (isset($allowed[$format]) || $format == $parentFormat) {
				$parents[$k] = ['title' => $v];
			} else {
				$parents[$k] = ['title' => $v, 'disabled' => true];
			}
		}

		return NLAbstractHTMLForm::BuildParentDropDown($field->name, $value, $parents);
	}
}

