<?php
class ObjectChecker {
	var $acl;

	public function checkAcl($acl, $what, $format, $object) {
		$this->acl = $acl;

		$cond[] = ($format->id == 'suser' && $object['id'] == $this->acl->user->id);
		$cond[] = ($format->id == 'suser' || $format->id == 'sgroup') && (in_array('pxuserauth', getColFromTable(debug_backtrace(), 'function')));

		if((bool) array_sum($cond)) {
			return true;
		}

		return $this->_checkGroup($what, $format, $object);
	}

	private function _checkGroup($what, $format, $object) {
		$params = func_get_args();

		foreach(array("_checkObject", "_checkParent") as $method) {
			$r = call_user_func_array(array($this, $method), $params);

			if(!is_null($r)) {
				return $r;
			}
		}

		return false;
	}

	private function _checkObject($what, $format, $object) {
		foreach ($this->acl->rules as $a) {
			$notEqualWhat    = !($a['what'] == $what);
			$notNullGroup    = !is_null($a['sgroupid']);
			$notInUserGroups = !in_array($a['sgroupid'], $this->acl->user->groups);
			$notUser         = $a['objectrule'] != 'user';

			if ($notEqualWhat || ($notNullGroup && $notInUserGroups) || $notUser) {
				continue;
			}

			$objectIdOrNull     = ($a['objectid']     == NULL || $a['objectid']     == $object['id']);
			/*
				Warning ! Use parent object rules with caution ! Rule like "permit access to any object of type STRUCT with parent 123" will also
				allow access to ANY object types (even are not of STRUCT type or not with immediate parent of STRUCT type) nested somewhere in STRUCT object with id 123
			*/
			$objectParentOrNull = ($a['objectparent'] == NULL || $a['objectparent'] == getFromArray($object, 'parent'));
			$objectTypeOrNull   = ($a['objecttype']   == NULL || $a['objecttype']   == $format->id);

			$notDenyAccess      = !($a['access'] === 'deny');

			if($objectIdOrNull  && $objectParentOrNull && $objectTypeOrNull) {
				return $notDenyAccess;
			}
		}
	}

	private function _checkParent($what, $format, $object) {
		if ((bool) $format->parent && isset($object['parent']) && is_numeric($object['parent']) && $object['parent'] > 0) {
			
			$object['id'] = -1; // deny check by id when check by parent !
			$pKey         = $format->parent . $object['parent'];

			$parentFormat = $this->acl->db->types[$this->acl->db->types[$format->id]->parent];

			if((bool)$parentFormat->parent) {
				$this->_loadParent($format, $object, $pKey);
				$object['parent'] = isset($this->acl->parents[$pKey]) ? $this->acl->parents[$pKey] : null;
			} else {
			}

			return $this->_checkGroup($what, $parentFormat, $object);
		 }
	}

	private function _loadParent($format, $object, $pKey) {
		if(isset($this->acl->parents[$pKey])){
			return;
		}
		
		$tmpParents = $this->acl->db->query('SELECT parent FROM '.$format->parent.' WHERE id = '.$object['parent']);

		if(count($tmpParents)) {
			$this->acl->parents[$pKey] = current(current($tmpParents));
		}
	}
	
}
?>