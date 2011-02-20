<?php
require_once 'interface.inc';
class PXDatabaseNull implements IPXDatabase {
	public function setUser($user) {
	}

	public function tableExists($table) {
		return false;
	}

	public function LoadDirectoriesAutomatic(&$directories) {
	}
}
?>