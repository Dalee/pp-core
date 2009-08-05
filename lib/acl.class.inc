<?php

class PXObjectsACL {
	/** @var boolean */
	var $ruleclEnabled;

	/** @var PXDatabase */
	var $db;

	/** @var PXUser */
	var $user;

	/** @var Array */
	var $rules;

	function PXObjectsACL(&$db, &$user) {
		$this->db   =& $db;
		$this->user =& $user;

		$this->aclEnabled  = (PXRequest::GetHttpMethod() != 'CLI' && $db->tableExists('acl_objects'));

		$this->rules   = array();
		$this->parents = array();

		$this->reload();
	}

	function reload() {
		$this->getRules();
	}

	function check($what, $format, $object) {
		if (!$this->aclEnabled) {
			return true;
		}
	
		if (!is_object($format)) {
			$format = PXRegistry::getApp()->types[$format];
	
			if(is_null($format))
				FatalError("Undefined format!"); 
		}

		$cond1 = ($format->id == 'suser' && $object['id'] == $this->user->id);

		$cond2_1 = ($format->id == 'suser' || $format->id == 'sgroup');
		//AND
		$cond2_2 = in_array('PXAuthSecure', getColFromTable(debug_backtrace(), 'function'));
		
		if($cond1 || ($cond2_1 && $cond2_2))
			return true;

		return $this->checkGroup($what, $format, $object);
	}

	private function checkGroup($what, $format, $object) {
		foreach(array("checkObject", "checkParent") as $method) {
			$args = func_get_args();
			$r = call_user_func_array(array($this, $method), $args);
			if(!is_null($r)) return $r;
		}

		return false;
	}
	
	private function checkObject($what, $format, $object) {
		foreach ($this->rules as $a) {
			if(($a['what'] != $what)
				|| (!is_null($a['sgroupid']) && !in_array($a['sgroupid'], $this->user->groups))) {
				continue;
			}

			$null_or_value = create_function('$value', 'return is_null($value) ? $value : null;');

			if(($a['objectid'] == $null_or_value(getFromArray($object, 'id')))
			 &&($a['objectparent'] == $null_or_value(getFromArray($object, 'parent')))
			 &&($a['objecttype']   == $null_or_value($format->id))) {
				return !($a['access'] === 'deny');
			}
		}
	}

	private function checkParent($what, $format, $object) {
		if ((bool) $format->parent 
		 && isset($object['parent']) 
		 && is_numeric($object['parent']) 
		 && $object['parent'] > 0) {
				
			$pKey = $format->parent . $object['parent'];

			if (!array_key_exists($pKey, $this->parents)) {
				$this->loadParent($format, $object, $pKey);
			}

			$object['parent'] = $this->parents[$pKey];
			$format = $this->db->types[$this->db->types[$format->id]->parent];

			return $this->checkGroup($what, $format, $object);
		 }
	}

	private function loadParent($format, $object, $pKey) {
		$tmpParents = $this->db->query(
			'SELECT parent FROM '.$format->parent.' WHERE id = '.$object['parent']);

		if(count($tmpParents)) 
			$this->parents[$pKey] = current(current($tmpParents));
	}

	private function getRules($loadAll=false) {
		if (!$this->aclEnabled) {
			return;
		}

		$this->rules = $this->db->query($this->sqlLoadRulesStatement($loadAll));
	}

	private function sqlLoadRulesStatement($loadAll) {
		$query = 'SELECT * FROM acl_objects';

		if(!$loadAll) {
			$query .= ' WHERE (sgroupid IS NULL';

			if ($this->user && count($this->user->groups)) {
				$query .= ' or sgroupid IN ('.implode(',', $this->user->groups).')';
			}

			$query .= ')';
		}

		return $query .= ' ORDER BY sys_order';
	}
}
?>
