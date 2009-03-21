#!/usr/local/bin/php -q
<?php

ini_set('display_errors', 1);

require_once dirname(__FILE__).'/../lib/maincommon.inc';

$engine = new PXEngineSbin();
$engine->init();

$ARGV = $_SERVER['argv'];

function show_usage_and_exit() {
    $opts = array("-l                            list users",  
                  "-p <login>                    change password password", 
                  "-s <login>                    show login details", 
                  "-a <login> <access> <pass>    add login", 
                  "-d <login>                    disable account", 
                  "-e <login>                    enableaccount");

    die("\nUsage:\n\tuserctl.php [options]\n\nWhere options are:\n\t". join($opts, "\n\t") . "\n\n");
}



##
## MAIN
##

if (count($ARGV) < 2) {
    show_usage_and_exit();
}

# It doesn't support long options (sic!)
# $options = getopt("lp:s:a:d:e:", 
#                    array("list", "password:","show:","add:","disable:","enable:"));

$options = getopt("lp:s:a:d:e:");

foreach($options as $option => $value) {
    switch($option) {
        case 'l':
                $users = $engine->db->getObjects($engine->app->types['suser'], NULL);
                $format = "%25s\t%5s\t%10s\t%15s\n";

                printf($format, 'login', 'status', 'access', 'modified');
                printf(str_repeat("--", 42) . "\n");
                
                foreach($users as $user) {
                    printf($format, $user['title'], $user['status'], $user['access'], $user['sys_modified']);
                }
            break;

        case 'p':
            break;
        case 's':
            break;
        case 'a':
            break;
        case 'd':
            break;
        case 'e':
            break;
        default:
            show_usage_and_exit();
    }
}

?>
