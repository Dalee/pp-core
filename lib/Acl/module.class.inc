<?php
class ModuleChecker {
	public function checkAcl(&$acl, $what, $format, $object = NULL) {
		foreach ($acl->rules as $a) {
			if (($a['what'] != $what) || 
				(!is_null($a['sgroupid']) && !in_array($a['sgroupid'], $acl->user->groups)) ||
				($a['objectrule'] != 'module')) {
				continue;
			}
			
			if ($a['objecttype'] == $format->name) {
				return !($a['access'] === 'deny');
			}
		}
		return false;
	}
}
?>