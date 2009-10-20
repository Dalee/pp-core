<?php
class ModuleChecker {
	public function checkAcl(&$acl, $what, $format, $object = NULL) {
		foreach ($acl->rules as $a) {
			$notEqualWhat    = !($a['what'] == $what);
			$notNullGroup    = !is_null($a['sgroupid']);
			$notInUserGroups = !in_array($a['sgroupid'], $acl->user->groups);
			$notModule       = $a['objectrule'] != 'module';

			$equalType       = $a['objecttype'] == $format->name;
			$isNullType      = is_null($a['objecttype']);
			$notEqualAccess  = !($a['access'] === 'deny');

			if ($notEqualWhat || ($notNullGroup && $notInUserGroups) || $notModule) {
				continue;
			}

			if ($equalType || $isNullType) {
				return $notEqualAccess;
			}
		}

		return false;
	}
}
?>