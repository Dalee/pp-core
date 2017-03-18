<?php

$newParent = !empty($this->helper->options->parent) && is_numeric($this->helper->options->parent) ? $this->helper->options->parent : 0;
$dtype = $this->app->types[$this->helper->objectType];
$parentField = isset($dtype->fields['parent']) ? 'parent' : (isset($dtype->fields['pid']) ? 'pid' : null);
if ($newParent && $parentField) {
	foreach ($this->helper->objectIds as $id) {
		$object = $this->db->GetObjectById($dtype, $id);
		if (!$object || $object[$parentField] == $newParent)
			continue;

		$object[$parentField] = $newParent;
		$this->db->ModifyContentObject($dtype, $object);
	}
} else {
	$RESULT = PXEngineJSON::toError('Недопустимый родитель объекта');
}
