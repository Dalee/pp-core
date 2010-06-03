#!/usr/bin/php5 -q
<?php

/* ret codes
 * 0 - ok
 * 1 - show usage
 * 2 - missed params
 * 3 - incorrect params
 * 255 - unknown error
 */


ini_set('display_errors', 1);

require_once dirname(__FILE__).'/../lib/maincommon.inc';

if(file_exists($localLib = dirname(__FILE__).'/../../local/lib/mainsbin.inc')){
        include_once $localLib;
}

$engine = new PXEngineSbin();

$ARGV = $_SERVER['argv'];

function show_usage_and_exit() {
	$opts = array(
		"-l                            list users",  
		"-g                            get auth type",
		"-p <login> <password>         change password",
		"-s <login>                    show login details",
		"-a <login> <access> <pass>    add login",
		"-d <login>                    disable account", 
		"-e <login>                    enable account"
	);

	echo("\nUsage:\n\tuserctl.php [options]\n\nWhere options are:\n\t". join($opts, "\n\t") . "\n\n");
	exit(1);
}

function missed_param($params = array()){
	$str    = count($params) > 1 ? join($params, '>, <') : $params[0];
	$need_s = count($params) > 1 ? "s" : "";
	echo("Missing required parameter{$need_s} <" . $str . ">\n");
	exit(2);
}

function incorrect_param($message = ''){
	echo $message;
	exit(3);
}

function success($message = ''){
	echo $message;
	exit(0);
}

##
## MAIN
##

function get_auth_method(){
	foreach(PXRegistry::getApp()->authrules as $rule => $param){
		if($param['enabled'] && (int)$param['enabled'] == 1){
			$amethod = $rule;
			break;
		}
	}

	if(!$amethod) {
		$amethod = 'null';
	}

	return $amethod;
}

function get_user_by_login($login){
	return current(PXRegistry::getDb()->getObjectsByWhere(PXRegistry::getApp()->types['suser'], NULL, "title = '" . $login . "'"));
}

function get_encoded_password($pass){
	$authtype = get_auth_method();
	require_once(BASEPATH . '/libpp/lib/User/Auth/classes.inc');
	return call_user_func(array('PXAuth' . ucfirst($authtype), 'passwdToDB'), $pass);
}

if (count($ARGV) < 2) {
	show_usage_and_exit();
}

# It doesn't support long options (sic!)
# $options = getopt("lp:s:a:d:e:", 
#                    array("list", "password:","show:","add:","disable:","enable:"));

$options = getopt("lgp:s:a:d:e:");

if(empty($options)){
	show_usage_and_exit();
}

# vars

$app = PXRegistry::getApp();
$db  = PXRegistry::getDb();

foreach($options as $option => $value) {
	switch($option) {
		case 'l':
			$users = $db->getObjects($app->types['suser'], NULL);
			$format = "%25s\t%5s\t%10s\t%15s\n";

			printf($format, 'login', 'status', 'access', 'modified');
			printf(str_repeat("--", 42) . "\n");

			foreach($users as $user) {
				printf($format, $user['title'], $user['status'], $user['access'], $user['sys_modified']);
			}

			echo "Total: " . count($users) . " users\n";
			success();
			break;

		case 'p':
			$login = $ARGV[2];
			$userdata = get_user_by_login($login);
			if($userdata){
				if(!isset($ARGV[3])) {
					missed_param(array('password'));
				}

				if(!isValidPasswd($ARGV[3])) {
					incorrect_param("Password must be from 3 to 16 symbols and contain only alphabetical, digits, \".\", \"-\", and \"_\"\n");
				}

				$userdata['passwd'] = get_encoded_password($ARGV[3]);
				$db->ModifyContentObject($app->types['suser'], $userdata);
				success("Password for user <{$login}> updated\n");
			} else {
				incorrect_param("User <{$login}> not found\n");
			}
			break;

		case 's':
			if(!$ARGV[2]) {
				missed_param('login');
			}

			$login = $ARGV[2];
			$userdata = get_user_by_login($login);

			if($userdata){
				$data = array(
					'Login:       ' . $userdata['title'],
					'Status:      ' . $userdata['status'],
					'Access:      ' . $userdata['access'],
					'Last modify: ' . $userdata['sys_modified']
				);

				success("\t" . join($data, "\n\t") . "\n");
			} else {
				incorrect_param("User <{$login}> not found\n");
			}
			break;

		case 'a':
			$errors = array();

			// check for args
			if(!isset($ARGV[2])) $errors[] = 'login';
			if(!isset($ARGV[3])) $errors[] = 'access';
			if(!isset($ARGV[4])) $errors[] = 'pass';

			if($errors){
				missed_param($errors);
			} else {
				//check for correct args
				$errors = '';

				if(!isValidLogin($ARGV[2])) {
					$errors .= "Login must be from 2 to 16 symbols and contain only alphabetical, digits, \".\", \"-\", and \"_\"\n";
				}

				if(!isValidPasswd($ARGV[4])) {
					$errors .= "Password must be from 3 to 16 symbols and contain only alphabetical, digits, \".\", \"-\", and \"_\"\n";
				}

				$check = get_user_by_login($ARGV[2]);

				if($check) {
					$errors .= "User <{$ARGV[2]}> already exists\n";
				}

				if($errors) {
					incorrect_param($errors);
				}

				$login  = $ARGV[2];
				$passwd = $ARGV[4];
				$access = intval($ARGV[3]);

				$blank = $app->initContentObject('suser');

				$blank['title']  = $login;
				$blank['passwd'] = get_encoded_password($passwd);
				$blank['access'] = $access;
				$blank['status'] = true;
				$db->addContentObject($app->types['suser'], $blank);
				success("User <{$login}> added successfully\n");
			}
			break;

		case 'd':
			$login = $ARGV[2];
			$userdata = get_user_by_login($login);

			if($userdata){
				if($userdata['status'] != 1) {
					incorrect_param("User <{$login}> already disabled\n");
				} else {
					$userdata['status'] = false;
					$db->ModifyContentObject($app->types['suser'], $userdata);
					success("User <{$login}> disabled\n");
				}

			} else {
				incorrect_param("User <{$login}> not found\n");
			}
			break;

		case 'e':
			$login = $ARGV[2];
			$userdata = get_user_by_login($login);
			if($userdata){
				if($userdata['status'] == 1) {
					incorrect_param("User <{$login}> is active\n");
				} else {
					$userdata['status'] = true;
					$db->ModifyContentObject($app->types['suser'], $userdata);
					success("User <{$login}> enabled\n");
				}

			} else {
				incorrect_param("User <{$login}> not found\n");
			}
			break;

		case 'g':
			$amethod = get_auth_method();
			success("Auth method is: " . $amethod . "\n");
			break;

		default:
			show_usage_and_exit();
	}
}
?>