<?php

class PXStorageTypeSQL extends PXStorageType
{
	public function createSelector($field, $object, $param = null)
	{
		$args = $this->field->storageTypeArgs;

		if (sizeof($args) != 1 && !strlen($args[0])) {
			FatalError('Not set arguments for sql storageType');
		}

		return '(' . $args[0] . ') as ' . PXRegistry::getDB()->mapFields($field->name);
	}

	protected static function parseConfigArguments($attrNode, $fieldNode)
	{
		$args = [];
		$params = $fieldNode->xpath('storageType');

		if (sizeof($params) > 0) {
			foreach ($params as $value) {
				$args[] = $value->nodeValue();
			}
		}

		return $args;
	}
}
?>
