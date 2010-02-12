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
	
	function __construct(&$db, &$user) {
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

		$checker = CheckerObject::get($format);
		return $checker->checkAcl($this, $what, $format, $object);
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
			$query .= " WHERE (sgroupid IS NULL";

			if ($this->user && count($this->user->groups)) {
				$query .= ' or sgroupid IN ('.implode(',', $this->user->groups).')';
			}

			$query .= ')';
		}

		return $query .= ' ORDER BY sys_order';
	}
}
?>