<?php
class PXTriggerStoragetypeVersion extends PXAbstractStoragetypeTrigger {
	function normalizeFilePath($field, &$object, &$vars) {
		if(isset($object['sys_version'])) {
			$p = array(
				'format'  => $vars['format'],
				'id'      => isset($object['sys_original']) ? $object['sys_original'] : $object['id'],
				'version' => $object['sys_version'],
				'name'    => $vars['name']
			);
			$vars = $p;
		}
	}
}
?>
