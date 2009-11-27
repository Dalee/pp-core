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
		foreach(array("checkObject", "checkParent") as $method) {
			$params = func_get_args();
			$r = call_user_func_array(array($this, $method), $params);
			if(!is_null($r)) return $r;
		}

		return false;
	}

	private function checkObject($what, $format, $object) {
		foreach ($this->acl->rules as $a) {
			if (($a['what'] != $what) || (!is_null($a['sgroupid']) && !in_array($a['sgroupid'], $this->acl->user->groups)) || ($a['objectrule'] != 'user')) {
				continue;
			}

			if( in_array($a['objectid'],        array($object['id'], NULL)) && in_array($a['objectparent'], array(getFromArray($object, 'parent'), NULL)) && in_array($a['objecttype'],   array($format->id, NULL))) {
				return !($a['access'] === 'deny');
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