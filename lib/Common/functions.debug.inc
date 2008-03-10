<?php

require_once('Error/reporter.class.inc');

function DebugText($text, $mode='H') {
	switch ($mode) {
		case 'H':
			print '<PRE>string('.strlen($text).')'.$text.'</PRE>';
			break;

		case 'T':
			print 'string('.strlen($text).')'.$text."\n";
			break;
	}
}

function DebugObject($object, $mode='H') {
	switch ($mode) {
		case 'H':
			print '<PRE>';
			echo var_dump($object);
			print '</PRE>';
			break;
		case 'T':
			echo var_dump($object);
			break;
	}
}

function DebugBool($isTrue) {
	if ($isTrue) {
		echo '<PRE>TRUE'."\n".'</PRE>';
	} else {
		echo '<PRE>FALSE'."\n".'</PRE>';
	}
}

function D($variable) {
	echo "\n";
	if (is_bool($variable)) {
		DebugBool($variable);
	} elseif (is_object($variable)) {
		DebugObject($variable);
	} elseif (is_array($variable)) {
		DebugObject($variable);
	} elseif (is_null($variable)) {
		DebugText("Variable IS NULL");
	} else {
		DebugText($variable);
	}
	echo "\n";
}

function D2($variable, $full=true) {
	echo "\n";
	$text = D3($variable, true, $full);

	if(substr(PHP_OS, 0, 3) == 'WIN') {
		$text = convert_cyr_string($text, 'k', (isset($_SERVER['REQUEST_METHOD']) ? 'w' : 'd'));
	}

	echo $text;
	echo "\n";
}

function D3_type2string($v) {
	switch(gettype($v)) {
		case 'resource':
			$d = 'Resource '.get_resource_type($v);
			break;

		case 'array':
			$d = 'Array '.sizeof($v);
			break;

		case 'object':
			$d = 'Object '.get_class($v);
			while(($v = get_parent_class($v)) !== false) {
				$d .= ' extend '.$v;
			}
			break;

		case 'boolean':
			$d = $v ? 'true' : 'false';
			break;

		case 'string':
			$d = "'$v'";
			break;

		case 'NULL':
			$d = 'NULL';
			break;

		default:
			$d = (string)$v;
			break;
	}

	return $d;
}

class D3EmptyObject {
}

function D3($variable, $withPre=true, $full=true) {
	if(!$full) {
		switch(true) {
			case is_array($variable):
				$showAble = array();

				foreach($variable as $k=>$v) {
					$showAble[$k] = D3_type2string($v);
				}

				$variable = $showAble;
				break;

			case is_object($variable):
				$showAble = new D3EmptyObject();

				$methods = array();
				foreach(get_class_methods($variable) as $k=>$v) {
					$methods[] = $v;
				}
				$showAble->_methods = $methods;

				foreach(get_object_vars($variable) as $k=>$v) {
					$showAble->$k = D3_type2string($v);
				}

				$variable = $showAble;
				break;
		}
	}

	if(is_null($variable)) {
		$variable = 'NULL';
	}

	if(is_bool($variable)) {
		$variable = $variable ? 'TRUE' : 'FALSE';
	}

	$debug = print_r($variable, true);
	if(isset($_SERVER['REQUEST_METHOD']) && $withPre) {
		$debug = htmlentities($debug, ENT_NOQUOTES, 'KOI8-R');
	}

	return (isset($_SERVER['REQUEST_METHOD']) && $withPre) ? '<pre class="debug">'.$debug.'</pre>' : $debug;
}

function DebugMicroTime($iden, $deal) {
	return NLProfiler($iden, $deal);
}

function NLProfiler($iden, $deal) {
	static $trace = array();
	static $level = 0;
	static $last = array();

	switch ($deal) {
		case 'BEGIN':
			$trace[$iden]['begin'] = GetMicroTime();
			$last[] = $iden;
			$level++;
			break;

		case 'END':
			end($last);
			$tmp = current($last);
			if($iden == $tmp) {
				array_pop($last);
				$level--;
			}
			$trace[$iden]['level'] = $level;
			$trace[$iden]['end'] = GetMicroTime();
			break;

		case 'SHOW':
			return ($trace[$iden]['end'] - $trace[$iden]['begin']);

		case 'ALL':
			$tmp = array();
			foreach($trace as $iden=>$time) {
				$tmp[$iden] = $time;
				$tmp[$iden]['long'] = NLProfiler($iden, 'SHOW');
			}
			return $tmp;
			break;
	}


	return 0;

}

function FatalError($errorMessage='', $errorType = 'FatalError') {
	NLErrorReporter::FatalError($errorMessage, $errorType);
	exit;
}

function GetMicroTime() {
	list($usec, $sec) = explode(" ", microtime());
	return ((float)$usec + (float)$sec);
}

function Label($str, $alwaysPrint=false) {
	if(isset($_SERVER['REQUEST_METHOD']) && !$alwaysPrint) {
		return;
	}

	static $start;

	if(!isset($start)) {
		$start = GetMicroTime();
	}

	$timer = sprintf('% 4.2f', GetMicroTime() - $start);

	if(strlen($timer) < 7) {
		$timer = str_repeat(' ', (7 - strlen($timer))).$timer;
	}

	if(IS_WIN) {
		$str = convert_cyr_string($str, 'k', 'd');
	}

	echo "\r".$timer."\t".$str."\n";

	return $timer;
}

function WorkProgress($finish=FALSE, $total=0) {
	static $progress, $counter;

	if(isset($_SERVER['REQUEST_METHOD']))  {
		return;
	}

	if(!isset($progress)) {
		$progress = '+';
	}

	if(!isset($counter)) {
		$counter = 0;
	}

	if($finish) {
		print "\r";
		$progress = '+';
		$counter = 0;
		return;
	}

	switch($progress) {
		case '+':
			$progress = '/';
			break;

		case '/':
			$progress = '-';
			break;

		case '-':
			$progress = '\\';
			break;

		case '\\':
			$progress = '+';
			break;
	}
	$counter++;

	print "\r".$progress."\t".$counter;

	if($total > 0) {
		print " of ".$total;
	}

	return $counter;
}


?>
