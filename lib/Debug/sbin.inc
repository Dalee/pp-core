<?php
// Выводит сообщение с указанием времени выполнения с точностью до десятков миллисекунд
function Label($str, $alwaysPrint=false) {
	if(isset($_SERVER['REQUEST_METHOD']) && !$alwaysPrint) {
		return;
	}

	static $start;

	if(!isset($start)) {
		$start = getMicroTime();
	}

	$timer = sprintf('% 4.2f', getMicroTime() - $start);

	if(strlen($timer) < 7) {
		$timer = str_repeat(' ', (7 - strlen($timer))).$timer;
	}

	if(IS_WIN) {
		$str = convert_cyr_string($str, 'k', 'd');
	}

	echo "\r".$timer."\t".$str."\n";

	return $timer;
}

// Отображает индикатор, считающий циклы в циклах =)
function WorkProgress($finish=FALSE, $total=0, $step = 1) {
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

	if(++$counter % $step) {
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

	print "\r".$progress."\t".$counter;

	if($total > 0) {
		print " of ".$total;
	}

	return $counter;
}

?>