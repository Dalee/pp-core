<?php

class PXDisplayTypeSysDeferredList extends PXDisplayTypeRadioList
{

	public function buildInput($field, $object, $param = NULL)
	{

		$object[$field->name] = PXDeferredStatusHelper::normalizeState($object[$field->name], $field->defaultValue);

		// emulate directory with our own items
		$field->values = new PXDirectoryDescription(null);
		$field->values->displayField = 'title';
		$field->values->values = PXDeferredStatusHelper::values();
		$field->values->setLoaded(false);

		return parent::buildInput($field, $object, $param);
	}

	public function buildCell($field, $object, $param = NULL)
	{

		$values = PXDeferredStatusHelper::values();
		$state = PXDeferredStatusHelper::normalizeState($object[$field->name], 'off');
		$stateTag = PXDeferredStatusHelper::stateTag($state);
		$human = $values[$state]['title'];

		$since = strtotime($object['sys_deferred_since']);
		$until = strtotime($object['sys_deferred_until']);
		$sinceS = date(' d.m.y, H:i', $since);
		$untilS = date(' d.m.y, H:i', $until);

		switch (true) {
			case $stateTag === 'prepared':
				$class = 'shown';
				$info = $human . $sinceS;
				break;
			case !$until && ($stateTag === 'published' || $stateTag === 'done'):
				$class = 'shown';
				$pstate = PXDeferredStatusHelper::normalizeState('published');
				$phuman = $values[$pstate]['title'];
				$info = $phuman . ($since ? $sinceS : '');
				break;
			case $stateTag === 'published':
				$class = 'shown';
				$info = $human . ' до' . $untilS;
				break;
			case $stateTag === 'done':
				$class = 'hidden';
				$info = $human . $untilS;
				break;
			case $stateTag === 'off':
				$class = 'hidden';
				$info = $human;
				break;
			default:
				$class = '';
				$info = '';
		}

		return sprintf('<span class="%s">%s</span>', $class, $info);
	}

}
