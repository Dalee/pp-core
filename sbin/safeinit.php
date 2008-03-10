#!/usr/local/bin/php -q
<?

if (count($_SERVER['argv']) < 2) {
	echo "Please, give me directory name as argument\n\n";
	exit();
}

$_SERVER["DOCUMENT_ROOT"] = $_SERVER['argv'][1];
require_once dirname(__FILE__).'/../lib/mainadmin.inc';

function getApacheUser() {
	return 'nobody';
}

function getApacheGroup() {
	return 'nogroup';
}

function recurseChown($path, $uid, $gid) {
	$d = opendir($path);
	while (($file = readdir($d)) !== false) {
		if ($file != '.' && $file != '..') {
			$typepath = $path.'/'.$file ;
			if (filetype($typepath) == 'dir') {
				recurseChown($typepath, $uid, $gid);
			}
			chown($typepath, $uid);
			chgrp($typepath, $gid);
		}
	}
}

function checkVarDir($dir) {
	echo 'Checking '.$dir.'...';
	if (file_exists(BASEPATH.'/'.$dir)) {
		if (is_dir(BASEPATH.'/'.$dir)) {
			echo ' exists, isdir'."\n";

		} else {
			echo ' exists, notisdir'."\n";
			echo 'Removing '.$dir.'...';
			unlink(BASEPATH.'/'.$dir);
			echo ' done'."\n";
			echo 'Creating '.$dir.'...';
			mkdir(BASEPATH.'/'.$dir);
			echo ' done'."\n";
		}

	} else {
		echo ' not exists'."\n";
		echo 'Creating '.$dir.'...';
		mkdir(BASEPATH.'/'.$dir);
		echo ' done'."\n";
	}
}

function prompt($prompt, $default) {
	$stdin = fopen('php://stdin', 'r');

	if ($default) {
		print $prompt.':['.$default.']';
		$input = trim(fgets($stdin, 1024));
		$retVal = ($input != '') ? $input : $default;

	} else {
		$input = '';
		while ($input == '') {
			print $prompt.':['.$default.']';
			$input = trim(fgets($stdin, 1024));
		}

		$retVal = $input;
	}

	return $retVal;
}

if (posix_getuid() != 0) {
	echo "You must run this script as root\n\n";
	exit();
}

while ($apacheUser = prompt("Please enter USER name, than Apache runs", getApacheUser())) {
	$tmp = posix_getpwnam($apacheUser);
	if (is_array($tmp)) {
		$apacheUser = $tmp['uid'];
		break;

	} else {
		echo "No such user\n";
	}
}

while ($apacheGroup = prompt("Please enter GROUP name, than Apache runs", getApacheGroup())) {
	$tmp = posix_getgrnam($apacheGroup);

	if (is_array($tmp)) {
		$apacheGroup = $tmp['gid'];
		break;

	} else {
		echo "No such group\n";
	}
}

checkVarDir('var');
checkVarDir('var/ad');
checkVarDir('var/ad/in');
checkVarDir('var/ad/out');
checkVarDir('var/dbcache');
checkVarDir('var/smarty_cache');
checkVarDir('var/smarty_templates_c');

echo 'Chowning var/...';
recurseChown(BASEPATH.'/var', $apacheUser, $apacheGroup);
chown(BASEPATH.'/var', $apacheUser);
chgrp(BASEPATH.'/var', $apacheGroup);
echo ' done'."\n";

?>
