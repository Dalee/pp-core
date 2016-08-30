<?php

namespace PP\Lib\Interfaces;

interface DatabaseInterface {

	public function setUser($user);
	public function LoadDirectoriesAutomatic(&$directories);

}
