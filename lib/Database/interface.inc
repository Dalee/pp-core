<?php
interface IPXDatabase {
	public function setUser($user);
	public function tableExists($table);
	public function LoadDirectoriesAutomatic(&$directories);
}
?>