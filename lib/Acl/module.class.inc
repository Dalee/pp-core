<?php

class ModuleChecker {
	public function checkAcl($acl, $what, $format, $object = NULL) {
		foreach ($acl->rules as $a) {
			$notEqualWhat = $a['what'] != $what;
			$notNullGroup = !is_null($a['sgroupid']);
			$notInUserGroups = !in_array($a['sgroupid'], $acl->user->groups);
			$notModule = $a['objectrule'] != 'module';

			if ($notEqualWhat || ($notNullGroup && $notInUserGroups) || $notModule) {
				continue;
			}

			$equalType = $a['objecttype'] == $format->getName();
			$isNullType = is_null($a['objecttype']);
			$notDenyAccess = $a['access'] !== 'deny';

			if ($equalType || $isNullType) {
				return $notDenyAccess;
			}
		}

		return false;
	}
}
