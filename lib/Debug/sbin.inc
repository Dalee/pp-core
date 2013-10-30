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
		$str = myconv(DEFAULT_CHARSET, CHARSET_DOS, $str);
	}

	echo "\r".$timer."\t".$str."\n";

	return $timer;
}

// Отображает индикатор, считающий циклы в циклах =)
function WorkProgress($finish = FALSE, $total = 0, $step = 1, $showTimes = false) {
	static $progress, $counter, $lastTotal;
	static $timer, $firstStamp, $prevStamp;

	if (isset($_SERVER['REQUEST_METHOD'])) {
		return;
	}

	if (!isset($counter)) {
		$counter = 0;
	}

	if (!isset($progress)) {
		$progress = 0;
	}

	if (!isset($lastTotal)) {
		$lastTotal = 0;
	}

	if (!isset($timer)) {
		$timer = false;
	}

	if ($finish) {
		// clear string
		echo "\r";
		echo " \t".str_repeat(' ', min(strlen((string) $counter) + 6 + strlen((string) $lastTotal) + ($timer ? 50 : 0), 72));

		echo "\r";
		$progress = $counter = $lastTotal = 0;
		$timer = false;
		$firstStamp = 0;
		return;
	}

	if ($showTimes) {
		$timer = $showTimes;
	}

	if ($timer && !$firstStamp) {
		$firstStamp = microtime(1);
	}

	if ($total) {
		$lastTotal = $total;
	}

	if (++$counter % $step) {
		return;
	}

	$progresses = "|/-\\";
	$progress ++;

	echo "\r".$progresses[$progress % 4]."\t".$counter;
	if ($lastTotal > 0) {
		echo " of ".$lastTotal;
		if ($timer) {
			$lastStamp = microtime(1);
			$currentElapsed = $lastStamp-$prevStamp;
			$elapsed = $lastStamp-$firstStamp;
			printf('. elpsd / avrg / last / total / left: %02d:%02d / %.3fms / %.3fms / %02d:%02d / %02d:%02d   ',
				$elapsed/60,
				$elapsed%60,
				($elapsed/$counter)*1000,
				$currentElapsed*1000,
				($totalTime = $elapsed/$counter*$lastTotal)/60,
				$totalTime%60,
				($leftTime = $totalTime-$elapsed)/60,
				$leftTime%60
			);
			$prevStamp = $lastStamp;
		}
	} elseif ($timer) {
		$lastStamp = microtime(1);
		$currentElapsed = $lastStamp-$prevStamp;
		$elapsed = $lastStamp-$firstStamp;
		printf('. elpsd / avrg / last: %02d:%02d / %.3fms / %.3fms', $elapsed/60, $elapsed%60, ($elapsed/$counter)*1000, $currentElapsed*1000);
		$prevStamp = $lastStamp;
	}

	return $counter;
}

?>
