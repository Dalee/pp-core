<?php
/**
 * Console helpers (con_*)
 *
 * @version 0.1
 */

define('CON_COLOR_BLACK',      "\033[00;30m", 1);
define('CON_COLOR_DGRAY',      "\033[01;30m", 1);
define('CON_COLOR_RED',        "\033[00;31m", 1);
define('CON_COLOR_LRED',       "\033[01;31m", 1);
define('CON_COLOR_GREEN',      "\033[00;32m", 1);
define('CON_COLOR_LGREEN',     "\033[01;32m", 1);
define('CON_COLOR_BROWN',      "\033[00;33m", 1);
define('CON_COLOR_YELLOW',     "\033[01;33m", 1);
define('CON_COLOR_BLUE',       "\033[00;34m", 1);
define('CON_COLOR_LBLUE',      "\033[01;34m", 1);
define('CON_COLOR_PURPLE',     "\033[00;35m", 1);
define('CON_COLOR_LPURPLE',    "\033[01;35m", 1);
define('CON_COLOR_CYAN',       "\033[00;36m", 1);
define('CON_COLOR_LCYAN',      "\033[01;36m", 1);
define('CON_COLOR_LGRAY',      "\033[00;37m", 1);
define('CON_COLOR_WHITE',      "\033[01;37m", 1);

define('CON_BGCOLOR_BLACK',    "\033[40m", 1);
define('CON_BGCOLOR_RED',      "\033[41m", 1);
define('CON_BGCOLOR_GREEN',    "\033[42m", 1);
define('CON_BGCOLOR_YELLOW',   "\033[43m", 1);
define('CON_BGCOLOR_BLUE',     "\033[44m", 1);
define('CON_BGCOLOR_MAGENTA',  "\033[45m", 1);
define('CON_BGCOLOR_CYAN',     "\033[46m", 1);
define('CON_BGCOLOR_WHITE',    "\033[47m", 1);

define('CON_COLS',             (`tput cols` - 10));

define('CON_EOC',              "\033[00m", 1);
define('CON_EOL',              CON_EOC . PHP_EOL);

function con_colors_array() {
	static $colors = [];

	if (empty($colors)) {
		foreach (get_defined_constants() as $k => $v) {
			if (mb_strpos($k, 'CON_COLOR_') === 0) {
				$colors['fg:'.mb_strtolower(mb_substr($k, 10))] = $v;
			} elseif (mb_strpos($k, 'CON_BGCOLOR_') === 0) {
				$colors['bg:'.mb_strtolower(mb_substr($k, 12))] = $v;
			}
		}
		$colors['end'] = CON_EOC;
	}

	return $colors;
}

function con_colors_debug() {
	$bgs = [];
	$fgs = [];
	foreach (get_defined_constants() as $k => $v) {
		if (mb_strpos($k, 'CON_COLOR_') === 0) {
			$fgs[mb_strtolower(mb_substr($k, 10))] = $v;
		} elseif (mb_strpos($k, 'CON_BGCOLOR_') === 0) {
			$bgs[mb_strtolower(mb_substr($k, 12))] = $v;
		}
	}

	// head
	echo str_repeat(' ', 12);
	foreach ($fgs as $fg => $fgc) {
		echo ($fg[0] == 'l' || $fg[0] == 'd')
			? (mb_strtoupper(mb_substr($fg, 0, 2)) . $fg[2])
			: mb_ucfirst(mb_substr($fg, 0, 3));
	}
	echo PHP_EOL;

	// colors table
	foreach ($bgs as $bg => $bgc) {
		echo mb_str_pad(mb_substr($bg, 0, 10), 10) . ': ';
		foreach ($fgs as $fg => $fgc) {
			echo $fgc . $bgc . ' aA';
		}
		echo CON_EOC . PHP_EOL;
	}
}

function con_message($message, $color = null, $wide = false) {
	$message = trim($message);
	// calc string length without colors
	$l = mb_strlen(preg_replace("@\033\\[(0[01];)?[0-9]{2}m|\\{([fb]g:[a-z]+|end)\\}@u", '', $message));
	$r = '';
	if ($wide) {
		$r .= $color . str_repeat(' ', $l + 6) . CON_EOL;
		$r .= $color . '  ' . $message . '    ' . CON_EOL;
		$r .= $color . str_repeat(' ', $l + 6) . CON_EOC;
	} else {
		$r .= $color . $message . CON_EOC;
	}
	return $r;
}

function con_vsprintf($message, array $args = []) {
	$r = vsprintf($message, $args);

	static $substs = [];
	if (empty($substs)) {
		$raw_substs = [
			'nl' => PHP_EOL,
			'eoc' => CON_EOC,
			'eol' => CON_EOL,
		];
		foreach ((con_colors_array() + $raw_substs) as $k => $v) {
			$substs['{'.$k.'}'] = $v;
		}
	}

	$r = str_replace(array_keys($substs), array_values($substs), $r);

	return $r;
}

function con_printf($message) {
	echo con_vsprintf($message, array_slice(func_get_args(), 1));
}

function _con_dump($args) {
	if (count($args) < 2 || mb_strpos($args[0], '%') !== false) {
		return;
	}
	con_printf('{fg:dgray}<<<"dump"{end}{nl}');
	array_map('var_dump', array_slice($args, 1));
	con_printf('{fg:dgray}dump;{end}{nl}{nl}');
}

/**
 * @param string $message
 * @param mixed $data,...
 */
function con_debug($message, $data = null) {
	$message = con_vsprintf($message, array_slice(func_get_args(), 1));
	echo con_message($message, CON_COLOR_LBLUE . CON_BGCOLOR_BLUE) . PHP_EOL;
	_con_dump(func_get_args());
}

/**
 * @param string $message
 * @param mixed $data,...
 */
function con_info($message, $data = null) {
	$message = con_vsprintf($message, array_slice(func_get_args(), 1));
	echo con_message($message, CON_COLOR_WHITE . CON_BGCOLOR_BLUE, 1) . PHP_EOL;
	func_num_args() > 1 && array_map('var_dump', array_slice(func_get_args(), 1));
}

/**
 * @param string $message
 * @param mixed $data,...
 */
function con_warn($message, $data = null) {
	$message = con_vsprintf($message, array_slice(func_get_args(), 1));
	echo con_message($message, CON_COLOR_WHITE . CON_BGCOLOR_RED) . PHP_EOL;
	func_num_args() > 1 && array_map('var_dump', array_slice(func_get_args(), 1));
}

/**
 * @param string $message
 * @param mixed $data,...
 */
function con_alert($message, $data = null) {
	$message = con_vsprintf($message, array_slice(func_get_args(), 1));
	echo con_message($message, CON_COLOR_WHITE . CON_BGCOLOR_RED, 1) . PHP_EOL;
	func_num_args() > 1 && array_map('var_dump', array_slice(func_get_args(), 1));
}

function con_eol($qty = 1) {
	echo CON_EOC . str_repeat(PHP_EOL, max($qty, 0));
}

function con_text($text) {
	echo $text . CON_EOL;
}

function con_head($title, $width = CON_COLS) {
	echo con_message(mb_str_pad(' ' . $title . ' ', $width, '=', STR_PAD_BOTH), CON_COLOR_LBLUE) . PHP_EOL;
}

function con_subhead($title, $width = CON_COLS) {
	echo con_message(mb_str_pad(' ' . $title . ' ', $width, '-', STR_PAD_BOTH), CON_COLOR_LGRAY) . PHP_EOL;
}

function con_table($caption, $data, $rows = null) {
	con_head($caption);
	if (empty($data)) {
		echo 'no data.' . PHP_EOL;
		return;
	}
	var_dump($data);die;
}

function con_list($list, $level = 0, $key_color = null, $value_color = null, $delim_color = null) {
	static $colors = [CON_COLOR_WHITE, CON_COLOR_YELLOW, CON_COLOR_LPURPLE, CON_COLOR_LGREEN, CON_COLOR_LRED];
	($key_color === null)   && $key_color   = $colors[$level+1];
	($value_color === null) && $value_color = CON_EOC;
	($delim_color === null) && $delim_color = $colors[$level];

	$max_key_length = $max_item_length = 6;
	foreach ($list as $key => $item) {
		$max_key_length = max($max_key_length, mb_strlen($key));
		!is_array($item) && $max_item_length = max($max_item_length, mb_strlen($item));
	}
	foreach ($list as $key => $item) {
		$str = str_repeat(' ', $level*2);
		$str .= $delim_color . '- ';
		$str .= $key_color   . $key;
		$str .= $delim_color . ': ' . str_repeat(' ', $max_key_length - mb_strlen($key));

		if (is_array($item)) {
			echo $str . CON_EOL;
			con_list($item, $level+1);
		} else {
			$str .= $value_color . ($item) . CON_EOL;
			echo $str;
		}
	}
}



function con_parse_args_string($s) {
	// parse
	$re = '@\s*(?P<param>([^\s\'"]+|"[^"]*"|\'[^\']*\')+)(?: |$)@';
	preg_match_all($re, $s . ' ', $m);
	$result = $m['param'];

	// remove quotes in each param
	$result = array_map(function($s) {
		return preg_replace('@\'([^\']*)\'|"([^"]*)"@', '\1\2', $s);
	}, $result);

	// done
	return $result;
}

/**
 * args parser
 *
 * @param string|array $argv
 * @param array $aliases
 * @return array
 */
function con_parse_args($argv = null, array $aliases = []) {

	// normalize params
	$aliases = $aliases ?: [];
	null === $argv    && $argv = $_SERVER['argv'];
	!is_array($argv)  && $argv = con_parse_args_string((string)$argv);

	$data = [
		'flags'   => [],
		'options' => [],
		'params'  => [],
	];

	// remove script name from args
	if (!empty($argv) && ($script = BASEPATH.$argv[0])
		&& file_exists($script)
		&& is_executable($script)
		&& mb_strpos(realpath($script), BASEPATH) === 0) {
		array_shift($argv);
	}

	if (empty($argv)) {
		return $data;
	}

	//echo con_info('in');var_dump($argv);die;

	// todo: move it to some class maybe?
	$findalias = function ($key) use ($aliases) {
		foreach ($aliases as $k => $v) {
			if ($v == $key || is_array($v) && in_array($key, (array)$v)) {
				$key = $k;
				break;
			}
		}
		$key = in_array($key, $aliases) ?: $key;
		return str_replace('_', '_', $key);
	};

	$setflag = function ($flag) use (&$data, $findalias) {
		$flag = $findalias($flag);
		empty($data['flags'][$flag]) && $data['flags'][$flag] = 0;
		$data['flags'][$flag] ++;
	};

	$setoption = function ($option, $value) use (&$data, $findalias) {
		$option = $findalias($option);
		$data['options'][$option] = $value;
	};

	// parse args array
	$key = null;
	$i = 0;
	while ($arg = array_shift($argv)) {
		$havekey    = null !== $key;
		$iskey      = !empty($arg) && $arg[0] == '-';
		$isshortkey = $iskey && isset($arg[1]) && $arg[1] != '-';
		$islongkey  = $iskey && isset($arg[1]) && $arg[1] == '-' && isset($arg[2]) && $arg[2] != '-';

		switch (true) {
			// set option for stored key
			case !$iskey && $havekey:
				$setoption($key, $arg);
				$key = null;
				break;
			// args
			default:
			case !$iskey:
				$data['params'][] = $arg;
				break;
			// push flag and try arg again
			case $isshortkey && $havekey:
				$setflag($key);
				$key = null;
				array_unshift($argv, $arg);
				break;
			// oneletter option with value
			case $isshortkey && mb_strlen($arg) > 2 && $arg[2] == '=':
				[$k, $v] = explode('=', mb_substr($arg, 1), 2);
				$setoption($k, $v);
				break;
			// flags pack in one arg: -abcdef
			case $isshortkey && mb_strlen($arg) > 2:
				for ($i=1, $l=mb_strlen($arg); $i<$l; $i++) {
					$setflag($arg[$i]);
				}
				break;
			// oneletter option: -f 22
			case $isshortkey:
				$key = $arg[1];
				break;
			// long option with value
			case $islongkey && mb_strlen($arg) > 3 && mb_strpos($arg, '='):
				[$k, $v] = explode('=', mb_substr($arg, 2), 2);
				$setoption($k, $v);
				break;
			// long param key
			case $islongkey:
				$key = mb_substr($arg, 2);
				break;
		}
	}

	!empty($key) && $setflag($key);

	return $data;
	//echo con_info('out');var_dump($data);die;
}



// & error hanler
function con_error_handler($errno, $errstr, $errfile, $errline, $errcontext) {
	$verbose = console::flag('verbose');
	if ((!$verbose) || ($verbose < 2 && ($errno & (E_USER_NOTICE|E_NOTICE|E_DEPRECATED|E_USER_DEPRECATED|E_STRICT)))) {
		return true;
	}

	static $errortypes = [
		E_ERROR              => ['Fatal Error',            '{fg:lred}'],
		E_WARNING            => ['Warning',                '{fg:brown}'],
		E_PARSE              => ['Parsing Error',          '{bg:red}{fg:white}'],
		E_NOTICE             => ['Notice',                 '{fg:dgray}'],
		E_CORE_ERROR         => ['Core Error',             '{fg:lred}'],
		E_CORE_WARNING       => ['Core Warning',           '{fg:brown}'],
		E_COMPILE_ERROR      => ['Compile Error',          '{fg:lred}'],
		E_COMPILE_WARNING    => ['Compile Warning',        '{bg:red}{fg:white}'],
		E_USER_ERROR         => ['User Error',             '{fg:lred}'],
		E_USER_WARNING       => ['User Warning',           '{fg:brown}'],
		E_USER_NOTICE        => ['User Notice',            '{fg:dgray}'],
		E_STRICT             => ['Runtime Notice',         '{fg:lgreen}'],
		E_RECOVERABLE_ERROR  => ['Catchable Fatal Error',  '{fg:lred}'],
		E_DEPRECATED         => ['Deprecated Notice',      '{fg:lblue}'],
		E_USER_DEPRECATED    => ['User Deprecated Notice', '{fg:lblue}'],
	];

	$errname = @$errortypes[$errno][0] ?: 'Unknown Error '.$errno;
	$errcol  = @$errortypes[$errno][1] ?: '';

	con_printf('%s%s: %s{eol}', $errcol, $errname, $errstr);
	con_printf('%1$s  %2$s on line %3$d{eol}', '{fg:dgray}', $errfile, $errline);
	con_printf('%s%s{eol}', $errcol, json_encode($errcontext));

	return true;
}
