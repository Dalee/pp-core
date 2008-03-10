<?php
/**
 * Project:     NetLogic PHP-classes library
 * File:        dbadatabase.class.inc
 *
 * @copyright   NetLogic, 2003-2006
 * @author      Stanislav Tugivikov <st@netlogic.ru>
 * @package     Netlogic
 * @subpackage  Database
 * @version     $Id$
 */

class NLDBADatabase {
	var $path;
	var $type;
	var $handle;
	var $mode;

	function NLDBADatabase($path, $type = "db3") {
		$this->path = $path;
		$this->type = $type;
		$this->mode = "n";
	}

	function Create() {
		if (!$this->handle = dba_open($this->path, "n", $this->type)) FatalError(ERROR_DB_CREATE, $this->path);
		dba_close($this->handle);
	}

	function SetReadMode() {
		switch ($this->mode) {
			case 'w':
				dba_close($this->handle);
				if (!$this->handle = @dba_open($this->path, "r", $this->type)) FatalError(ERROR_DB_OPEN, $this->path);
				break;
			case 'r':
				// do nothing
				break;
			case 'n':
				if (!$this->handle = @dba_open($this->path, "r", $this->type)) FatalError(ERROR_DB_OPEN, $this->path);
				break;
		}
		$this->mode = "r";
	}

	function SetWriteMode() {
		switch ($this->mode) {
			case 'r':
				dba_close($this->handle);
				if (!$this->handle = @dba_open($this->path, "w", $this->type)) FatalError(ERROR_DB_OPEN, $this->path);
				break;
			case 'w':
				// do nothing
				break;
			case 'n':
				if (!$this->handle = @dba_open($this->path, "w", $this->type)) FatalError(ERROR_DB_OPEN, $this->path);
				break;
		}
		$this->mode = "w";
	}

	function SetNoneMode() {
		if ($this->handle) dba_close($this->handle);
		$this->mode = "n";
	}

	function Select($key) {
		$this->SetReadMode();
		$value = dba_fetch($key, $this->handle);
		return $value;
	}

	function SelectAll($regex = NULL) {
		$this->SetReadMode();
		$values = array();
		$key = dba_firstkey($this->handle);
		if (!$key) $key = dba_nextkey($this->handle);
		if ($key) {
			if (!$regex || ereg($regex, $key)) $values[$key] = dba_fetch($key, $this->handle);
			while ($key = dba_nextkey($this->handle))
				if (!$regex || ereg($regex, $key)) $values[$key] = dba_fetch($key, $this->handle);
		}
		return $values;
	}

	function Insert($key, $value) {
		$this->SetWriteMode();
		dba_replace($key, $value, $this->handle);
		$this->SetNoneMode();
	}

	function Update($key, $value) {
		$this->SetWriteMode();
		dba_replace($key, $value, $this->handle);
		$this->SetNoneMode();
	}

	function Delete($key) {
		$this->SetWriteMode();
		dba_delete($key, $this->handle);
	}

	function Count() {
		$this->SetReadMode();
		$key = dba_firstkey($this->handle);
		$count = 0;
		do {
			$count++;
		} while (($key = dba_nextkey($this->handle)) !== false);
		return $count;
	}

}

?>
