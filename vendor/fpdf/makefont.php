#!/usr/bin/env php5
<?php

(PHP_SAPI !== 'cli') && die ('Only for cli'.PHP_EOL);

include "makefont/makefont.php";

// Command-line interface
($argc == 1) && die("Usage: php makefont.php fontfile [enc:koi8-r] [embed:true]\n");

$font  = $argv[1];
$enc   = !empty($argv[2]) ? $argv[2] : 'koi8-r';
$embed = empty($argv[3]) || $argv[3] == 'true' || $argv[3] == '1';

MakeFont($font, $enc, $embed);
