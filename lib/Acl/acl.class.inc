<?php

use PP\Lib\Database\Driver\PostgreSqlDriver;

class PXObjectsACL
{

	/** @var bool */
	public $ruleclEnabled;

	/** @var PXDatabase|PostgreSqlDriver */
	public $db;

	/** @var PXUser|PXUserAuthorized */
	public $user;

	/** @var array */
	public $rules;

	/**
	 * PXObjectsACL constructor.
	 *
	 * @param PXDatabase $db
	 * @param PXUser $user
	 */
	public function __construct(&$db, &$user)
	{
		$this->db =& $db;
		$this->user =& $user;

		$this->aclEnabled = (PXRequest::GetHttpMethod() != 'CLI' && $db->tableExists('acl_objects'));

		$this->rules = [];
		$this->parents = [];

		$this->reload();
	}

	public function reload()
	{
		$this->_getRules();
	}

	public function check($what, $format, $object)
	{
		if (!$this->aclEnabled) {
			return true;
		}

		return CheckerObject::get($format)->checkAcl($this, $what, $format, $object);
	}

	private function _getRules($loadAll = false)
	{
		if (!$this->aclEnabled) {
			return;
		}

		$this->rules = $this->db->query($this->_sqlLoadRulesStatement($loadAll));
	}

	private function _sqlLoadRulesStatement($loadAll)
	{
		$query = 'SELECT * FROM acl_objects';

		if (!$loadAll) {
			$query .= " WHERE (sgroupid IS NULL";

			if ($this->user && count($this->user->groups)) {
				$query .= ' or sgroupid IN (' . implode(',', $this->user->groups) . ')';
			}

			$query .= ')';
		}

		return $query .= ' ORDER BY sys_order';
	}
}
