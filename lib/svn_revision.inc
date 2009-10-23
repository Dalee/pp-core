<?php
	//  change something in this file to change svn revision
	$svnid = '$Rev$';
	$scid = substr($svnid, 6);
	define('PP_REVISION', intval(substr($scid, 0, strlen($scid) - 2)));
?>