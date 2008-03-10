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

		$this->rules       = array();
		$this->parents     = array();

		$this->reload();
	}

	function reload() {
		$this->_getRules();
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
        if (!is_object($format))
           FatalError("Format must be type of PXTypeDescription!"); 
		
        if ($format->id == 'suser' && $object['id'] == $this->user->id) {
			return true;
		}

		if ($format->id == 'suser' || $format->id == 'sgroup') {
			if (in_array('pxuser', getColFromTable(debug_backtrace(), 'function'))) {
				return true;
			}
		}

		$isCan = false;

		foreach($this->user->groups as $groupId) {
			switch($this->_checkGroup($what, $format, $object, $groupId)) {
				case null:
					continue;
					break;

				case true:
					$isCan = true;
					break(2);

				case false:
					$isCan = false;
					break(2);
			}
		}

//		$this->debug($isCan, $object, $what, $format);

		return $isCan;
	}

	function debug($isCan, $object, $what, $format) {
		switch($what) {
			case 'read':
				if($format->id !== 'struct' || $object['id'] != 162) {
					return;
				}
				break;

			case 'admin':
				return;
				break;

			default:
				return;
		}

		$s = ($isCan ? 'Allow' : (is_null($isCan) ? 'Null' : 'Deny')) . ' / ' . $object['title'];
		d2($s);
	}



    /**
     *
     * FIXME: COZ I'M UGLY!
     *
     */
	function _checkGroup($what, $format, $object, $groupId = null) {
        
        //check access for current $object
        if ($groupId !== NULL) {
            foreach ($this->rules as $rule) {
                
                if ($rule['what'] != $what || $rule['sgroupid'] != $groupId) {
                    continue;
                }

                $checkId     = ($rule['objectid']     == NULL || $rule['objectid']     == $object['id']);
                $checkParent = ($rule['objectparent'] == NULL || $rule['objectparent'] == getFromArray($object, 'parent'));
                $checkType   = ($rule['objecttype']   == NULL || $rule['objecttype']   == $format->id);

                if ($checkId && $checkParent && $checkType) {
                    return ($rule['access'] !== 'deny');
                }
            }//foreach
        }//if

        //nothing found, let's check parent
		if ($format->parent && isset($object['parent']) && is_numeric($object['parent']) && $object['parent'] > 0) {
			$pKey = $format->parent.$object['parent'];

			if (!array_key_exists($pKey, $this->parents)) {
				$query = 'SELECT parent FROM '.$format->parent.' WHERE id = '.$object['parent'];
				$tmpParents = $this->db->query($query);

				if(!sizeof($tmpParents)) {
					return null;
				}

				$this->parents[$pKey] = current(current($tmpParents));
			}

			$object['parent'] = $this->parents[$pKey];
			$format = $this->db->types[$format->parent];

			return $this->_checkGroup($what, $format, $object, $groupId);
		}

		return null;
	}

	function _getRules($loadAll=false) {
		if (!$this->aclEnabled) {
			return;
		}

		$query = 'SELECT * FROM acl_objects';

		if(!$loadAll) {
			$query .= ' WHERE (sgroupid IS NULL';

			if ($this->user && count($this->user->groups)) {
				$query .= ' or sgroupid IN ('.implode(',', $this->user->groups).')';
			}

			$query .= ')';
		}

		$query .= ' ORDER BY sys_order';

		$this->rules = $this->db->query($query);
	}
}
?>
