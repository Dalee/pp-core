<?php

namespace PP\Lib\Database;

interface DatabaseInterface {

	public function setUser($user);
	public function LoadDirectoriesAutomatic(&$directories);

}
