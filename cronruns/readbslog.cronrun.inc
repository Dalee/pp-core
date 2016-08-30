<?php

// TODO: удалить этот пиздец, начать с unite
class PXCronRunReadBSLog extends PXAbstractCronRun {
	var $DATE_FORMAT   = 'd.m.Y H:00:00';              // date format here [d-m-Y H:i:s] german time format

	var $RELOAD_FILE   = '/site/var/ad/in/reload'; // Файл по которому делается релоад. Относительно BASEPATH
	var $BS_OUT_DIR    = '/site/var/ad/out/';      // Каталог с bs log files (входные). Относительно BASEPATH
	var $LOGS_DIR      = '/site/var/ad/logs/';     // Каталог с результатами работы скрипта (выход). Относительно BASEPATH

	protected $fLimit, $leaveSourceFile = false, $skipLog = false;

	function PXCronRunReadBSLog() {
		$this->name = 'Обработка логов баннерной системы';

		$this->result = array();
		$this->files  = array(); // массив с файлами статистики

		$this->audit  = array(); // статистика audit letters
		$this->stats  = array(); // статистика показов/кликов

		$this->badLines = array(); // испорченные строчки в логе

		$this->total  = array(
			'clicks' => 0,
			'shows'  => 0,
			'audit'  => 0
		);

		$this->timeMin = 0;
		$this->timeMax = 0;

		/*
			Extra console params:
			argv[2] means filename to parse or files limit per round
			argv[3] means skip write banner event data to log
		*/
		if (!empty($_SERVER['argv'][2])) {
			if (is_numeric($_SERVER['argv'][2])) {
				$this->fLimit = (int)$_SERVER['argv'][2];
			} else {
				$this->files[]   = $_SERVER['argv'][2];
				$this->leaveSourceFile = true;
			}

			$this->skipLog = !empty($_SERVER['argv'][3]);
		}
	}

	function Run(&$app, &$db, &$tree, $mTime, $mRule) {
		$this->db =& $db;

		Label('Starting...');

		ini_set('memory_limit', -1); // For really large lists of files

		$message = $this->touchReload();
		if(is_string($message)) {
			return array('status' => -1, 'note' => $message);
		}

		if (empty($this->files)) {
			$message = $this->filesList();
			if(is_string($message)) {
				return array('status' => -2, 'note' => $message);
			}
		}

		$message = $this->processing();
		if(is_string($message)) {
			return array('status' => -3, 'note' => $message);
		}

		$this->insertBannerStats();
		$this->insertLettersAudit();

		return array(
			'status' => 0,
			'note' => 'Успешно! (clicks/shows/audit: '.$this->total['clicks'].'/'.$this->total['shows'].'/'.$this->total['audit'].')'.(
				sizeof($this->badLines) > 0 ? "\n\n".'Ошибки:'."\n".implode("\n", $this->badLines) : ''
			)
		);
	}

	function touchReload() {
		if (!@touch(BASEPATH.$this->RELOAD_FILE)) {
			$message = 'Cannot touch file: '.$this->RELOAD_FILE;
			Label($message);
			return $message;
		}

		sleep(1);
	}

	function filesList() {
		Label('Get files list');
		$handle = @opendir(BASEPATH.$this->BS_OUT_DIR);
		if (!$handle) {
			$message = 'Cannot open dir: '.BASEPATH.$this->BS_OUT_DIR;
			Label($message);
			return $message;
		}

		while (false !== ($file = readdir($handle)) && (!isset($this->fLimit) || $this->fLimit-- > 0)) {
			WorkProgress();

			if (preg_match('/^log\.(\d+\.\d+)$/', $file, $match)) {
				$this->files[$match[1]] = BASEPATH.$this->BS_OUT_DIR.$file;
			}
		}
		WorkProgress(true);

		closedir($handle);
		ksort($this->files);
	}

	function processing() {
		if(!sizeof($this->files)) {
			$message = 'Nothing to do';
			Label($message);
			return $message;
		}

		Label('Processing '.sizeof($this->files).' files...');

		foreach ($this->files as $file) {
			$this->_parseFile($file);

			empty($this->leaveSourceFile) && unlink($file);
		}
		WorkProgress(true);
	}

	function _parseFile($file) {
		Label('Processing file: '.$file);

		$t = @stat($file);

		if (!$t['size']) {
			return;
		}

		$fileContents = file_get_contents($file);
		$fileStrings  = explode("\n", $fileContents);

		foreach ($fileStrings as $s) {
			WorkProgress();
			$this->_parseLine($s, $file);
		}
	}

	function _checkDate($date) {
		static $min;
		static $max;

		if(is_null($min)) {
			$min = mktime(0,0,0,1,1,2000);
		}

		if(is_null($max)) {
			$max = time();
		}
		return $date > $min && $date <= $max;
	}

	function _parseLine($s, $file) {
		if (empty($s)) {
			return;
		}

		$temp     = explode(" ", $s);
		$adplace  = $temp[3];
		$adbanner = $temp[4];

		if(!$this->_checkDate($temp[0])) {
			$this->badLines[] = "\t".$file."\t".$s;
			return;
		}

		$time     = date($this->DATE_FORMAT, $temp[0]);
		$action   = ($temp['2'] == 'clk') ? 'clk' : 'show';

		//calculate minimal and maximal time from new logs
		if ($this->timeMin != 0) {
			if ($this->timeMin >= $temp[0]) {
				$this->timeMin = $temp[0];
			}

		} else {
			$this->timeMin = $temp[0];
		}

		if ($this->timeMax != 0)  {
			if ($this->timeMax <= $temp[0]) {
				$this->timeMax = $temp[0];
			}

		} else {
			$this->timeMax = $temp[0];
		}

		//log it
		$logFileName = date('Y-m-d', $temp[0]).'.log';
		$this->_log($s."\n", $logFileName);

		//put to array ,-)
		switch (sizeof($temp)) {
			case 6: //v1 logs
			case 7:
				$this->_arrayInc($this->stats[$time][$adplace][$adbanner][$action]);
				break;

			case 8: //v2 logs
				if ($temp[7] === "audit") {
					$this->_arrayInc($this->audit[$adbanner]); //audit action here
				} else {
					$this->_arrayInc($this->stats[$time][$adplace][$adbanner][$action]);
				}
				break;

			default:
				//Label('Broken string found in file: [' . $file . '] at line: ['.$i.'] skipping...');
				break;
		}
	}

	function insertLettersAudit() {
		Label('Update audit letters stats...');

		//update audit letters stats
		foreach ($this->audit as $id => $shows) {
			WorkProgress();
			$this->db->modifyingQuery("UPDATE newsletter SET shows = shows + {$shows} WHERE id = {$id}", NULL, NULL, false);

			$this->total['audit'] += $shows;
		}
		WorkProgress(TRUE);
	}

	function insertBannerStats() {
		Label('Insert banners stats...');

		$db =& $this->db;

		$timeMin = date($this->DATE_FORMAT, $this->timeMin);
		$timeMax = date($this->DATE_FORMAT, $this->timeMax);

		$sql = "SELECT ts, adplace, adbanner FROM adstat WHERE ts BETWEEN '".$timeMin."' AND '".$timeMax."'";
		$rowsToUpdate = $db->query($sql, true);

		if(!is_array($rowsToUpdate)) {
			$rowsToUpdate = array();
		}

		$hashToUpdate = array();

		foreach($rowsToUpdate as $row) {
			$hashToUpdate[join('|', array($row["ts"], $row["adplace"], $row["adbanner"]))] = true;
		}

		Label('rowsToUpdate - '.sizeof($rowsToUpdate));
		unset($rowsToUpdate);

		//insert new banner stats into db
		foreach ($this->stats as $timestamp => $timestampData) {
			WorkProgress();

			foreach ($timestampData as $place => $placeData) {
				WorkProgress();

				foreach ($placeData as $banner => $actions) {
					WorkProgress();

					$show = isset($actions['show']) ? $actions['show'] : 0;
					$clk  = isset($actions['clk'])  ? $actions['clk']  : 0;

					// update existing record
					if (isset($hashToUpdate[join('|', array($timestamp, $place, $banner))])) {
						$sql = "UPDATE adstat SET show = show + '{$show}', click = click + '{$clk}' ";
						$sql.= "WHERE ts = '{$timestamp}' AND adplace = '{$place}' and adbanner='{$banner}'";
						$act = "U";

					// or insert new row
					} else {
						$sql = "INSERT INTO adstat (adplace, adbanner, ts, show, click) ";
						$sql.= "VALUES ('{$place}','{$banner}', '{$timestamp}', '{$show}', '{$clk}');";
						$act = "I";
					}

					Label ($timestamp ."\t". $place ."\t". $banner ."\t". $show ."\t". $clk ."\t[".$act."]");
					$db->modifyingQuery($sql, NULL, NULL, false);

					// and update adbanner table...
					if ($show > 0) {
						$db->modifyingQuery("UPDATE adbanner SET shows  = shows  + '{$show}' WHERE id = {$banner}", NULL, NULL, false);
						$this->total['shows'] += $show;
					}

					if ($clk > 0) {
						$db->modifyingQuery("UPDATE adbanner SET clicks = clicks + '{$clk}'  WHERE id = {$banner}", NULL, NULL, false);
						$this->total['clicks'] += $clk;
					}

				}
			}
		}
		WorkProgress(TRUE);
	}

	function _arrayInc(&$arr) {
		if (isset($arr)) {
			$arr++;
		} else {
			$arr = 1;
		}
	}

	function _log($str, $logNameTemp) {
		if ($this->skipLog) {
			return;
		}

		static $logHandler = null;
		static $logName    = '';

		if ($logName != $logNameTemp) {
			if ($logName != '') {
				fclose($logHandler);
			}

			$logName    = $logNameTemp;
			$logHandler = fopen (BASEPATH.$this->LOGS_DIR.$logName, 'a');

			Label('creating file '.$logName);
		}

		fwrite($logHandler, $str);
	}
}

/*

Описание форматов логов

version1:

	formal:
	[unix timestamp] [ip address] [action] [placeid] [bannerid] [apache uid]

version2:

	formal:
	[unix timestamp] [ip address] [action] [placeid] [bannerid] [apache uid] [type] [template]

	[unix timestamp] - обычный unix time от начала эпохи
	[ip address]     - IP адрес в виде nnn.nnn.nnn.nnn
	[action]         - ["show", "clk"] показ баннера, клик по баннеру
	[placeid]        - id места показа в БД
	[bannerid]       - id баннера в БД
	[apache uid]     - уникальный id апача, хз зачем он тут ,-)
	[type]           - тип баннера, или его размер "NNNxNNN"
	[template]       - шаблон вывода баннера [gif, flash, audit... ?]

	example:
	1097241447 192.168.1.21 show 281470 281471 5113442241610AF8000007BA03035D02 123x123 gif

*/
