<?php
class ObjectChecker {
	var $acl;

	public function checkAcl(&$acl, $what, $format, $object) {
		$this->acl = & $acl;

		if (!is_object($format)) {
			$format = PXRegistry::getApp()->types[$format];
	
			if(is_null($format)) {
				FatalError("Undefined format!"); 
			}
		}

		$cond[] = ($format->id == 'suser' && $object['id'] == $this->acl->user->id);
		$cond[] = ($format->id == 'suser' || $format->id == 'sgroup') && (in_array('pxuserauth', getColFromTable(debug_backtrace(), 'function')));

		if((bool) array_sum($cond)) {
			return true;
		}

		return $this->checkGroup($what, $format, $object);
	}

	private function checkGroup($what, $format, $object) {
		$params = func_get_args();

		foreach(array("checkObject", "checkParent") as $method) {
			$r = call_user_func_array(array($this, $method), $params);

			if(!is_null($r)) {
				return $r;
			}
		}

		return false;
	}

	private function checkObject($what, $format, $object) {
		foreach ($this->acl->rules as $a) {
			$notEqualWhat    = !($a['what'] == $what);
			$notNullGroup    = !is_null($a['sgroupid']);
			$notInUserGroups = !in_array($a['sgroupid'], $this->acl->user->groups);
			$notUser         = $a['objectrule'] != 'user';

			if ($notEqualWhat || ($notNullGroup && $notInUserGroups) || $notUser) {
				continue;
			}

			$objectIdOrNull     = in_array($a['objectid'],     array($object['id'],                   NULL));
			$objectParentOrNull = in_array($a['objectparent'], array(getFromArray($object, 'parent'), NULL));
			$objectTypeOrNull   = in_array($a['objecttype'],   array($format->id,                     NULL));

			$notDenyAccess      = !($a['access'] === 'deny');

			if($objectIdOrNull  && $objectParentOrNull && $objectTypeOrNull) {
				return $notDenyAccess;
			}
		}
	}

	private function checkParent($what, $format, $object) {
		if ((bool) $format->parent && isset($object['parent']) && is_numeric($object['parent']) && $object['parent'] > 0) {
			$pKey = $format->parent . $object['parent'];

			if (!array_key_exists($pKey, $this->acl->parents)) {
				$this->loadParent($format, $object, $pKey);
			}

			$object['parent'] = $this->acl->parents[$pKey];
			$format = $this->acl->db->types[$this->acl->db->types[$format->id]->parent];

			return $this->checkGroup($what, $format, $object);
		 }
	}

	private function loadParent($format, $object, $pKey) {
		$tmpParents = $this->acl->db->query('SELECT parent FROM '.$format->parent.' WHERE id = '.$object['parent']);

		if(count($tmpParents)) {
			$this->acl->parents[$pKey] = current(current($tmpParents));
		}
	}
	
}
?>