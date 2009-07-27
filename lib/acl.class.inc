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

		/*
		 * 
		 * We have a couple errors with usage of this functon.
		 * While It must be called as $...->check('action', $app->types['some-type'], $obj), 
		 * some times it called using type name - $...->check('action', 'some-type', $obj).
		 * 
		 * This error is silently ignored in PHP4.
		 *
		 */

		if (!is_object($format)) {
			FatalError("Format must be type of PXTypeDescription!"); 
		}

		$cond[] = ($format->id == 'suser' && $object['id'] == $this->user->id);
		$cond[] = ($format->id == 'suser' || $format->id == 'sgroup') && 
			(in_array('pxuserauth', getColFromTable(debug_backtrace(), 'function')));

		if((bool) array_sum($cond)) return true;
		return (bool) $this->checkGroup($what, $format, $object);
	}

	private function checkGroup($what, $format, $object) {
		//SHIT! write cond
		if(is_null($r1 = $this->checkObject($what, $format, $object)) return $r1;
		if(is_null($r2 = $this->checkParent($what, $format, $object)) return $r2;
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

	private function checkObject($what, $format, $object) {
		foreach ($this->rules as $a) {
			if (($a['what'] != $what) || 
				(!is_null($a['sgroupid']) && !in_array($a['sgroupid'], $this->user->groups)) {
				continue;
			}

			if(($a['objectid']     == (NULL || $object['id'])
				&& ($a['objectparent'] == (NULL || getFromArray($object, 'parent'))) 
				&& ($a['objecttype']   == (NULL || $format->id))) {
			
				return ($a['access'] === 'deny') ? false : true;
			}
		}
	}

	private function getRules($loadAll=false) {
		if (!$this->aclEnabled) {
			return;
		}

		$this->rules = $this->db->query($this->sqlLoadRulesStatement($loadAll));
	}

	function sqlLoadRulesStatement($loadAll) {
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
