<?php
/**
 * Нередактируемое поле
 *
 * storageType может быть любым, но как правило integer или string
 * displayType STATIC
 */
class PXDisplayTypeStatic extends PXDisplayType
{
	public function buildInput($field, $object, $param = NULL)
	{
		$k = $field->name;
		$value = isset($object[$k]) ? (is_array($object[$k]) ? print_r($object[$k], true) : $object[$k]) : NULL;

		$values = isset($field->values) ? $field->values->GetList() : [];
		return NLAbstractHTMLForm::BuildHidden($field->name, $value) . htmlspecialchars(isset($values[$value]) ? $values[$value] : $value, ENT_COMPAT | ENT_HTML401, DEFAULT_CHARSET);
	}

	public function buildCell($field, $object, $param = NULL)
	{
		$value = $object[$field->name];

		return htmlspecialchars($this->getSubDirectoryIfExists($field, $object, $value), ENT_COMPAT | ENT_HTML401, DEFAULT_CHARSET);
	}

	public function preModifyObject($field, &$object, $param = NULL)
	{
		return (bool)$param['skipHidden'];
	}
}
?>
