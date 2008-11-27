<?php
class PXModuleCronRun extends PXAbstractModule {
	var $TIME_FORMAT   = 'd-m-Y H:i:s';
	var $rules;

	function __construct($area, $settings) {
		parent::__construct($area, $settings);

		$this->rules    = array();
		$this->job2rule = array();
		$this->jobs     = array();

		$this->_parseRules($settings);
	}

	function _parseRules($settings) {
		if (!isset($settings['rule']) || !count($settings['rule'])) {
			return;
		}

		if (count($settings['rule']) == 1) {
			$settings['rule'] = array($settings['rule']);
		}

		$count = 0;
		foreach ($settings['rule'] as $s) {
			if (!preg_match("/^\s*(.+?)\s+(\w+?)\s*$/", $s, $m)) {
				continue;
			}

			$rule     = new PXCronRule($m[1]);
			$jobname  = $m[2];

			if (!$rule->valid) {
				continue;
			}

			$file = strtolower($jobname).".cronrun.inc";

			if (file_exists(BASEPATH.'/local/cronruns/'.$file)) {
				include_once BASEPATH.'/local/cronruns/'.$file;
			} elseif (file_exists(BASEPATH.'/libpp/cronruns/'.$file)) {
				include_once BASEPATH.'/libpp/cronruns/'.$file;
			} else {
				continue;
			}

			$class = 'PXCronRun' . $jobname;
			$rm = array(
				'rule'    => $rule,
				'jobhash' => $rule->matchHash.md5($jobname),
				'jobname' => strtolower($jobname),
				'job'     => new $class()
			);

			$this->rules[$count] = $rm;
			$this->jobs[$jobname] = &$this->rules[$count];
			$this->job2rule[$rm['jobhash']] = $count;
			$count++;
		}
	}

	function RunJob(&$job, &$app, $matchedTime) {
		$tmpDir = IS_WIN ? $_ENV['TEMP'] : '/tmp/';

		// ******* child code ******
		$fname = $tmpDir.'/cron'.md5($_SERVER["DOCUMENT_ROOT"]).$job['jobname'].'.lock';
		$fp = @fopen($fname, 'r');
		if (!$fp) $fp = fopen($fname, 'w');
		$st = flock($fp, LOCK_EX | LOCK_NB);
		if (!$st) {
			$fatal = true;
			if (isset($job['job']->longrunner)) {
				$tmp = intval($job['job']->longrunner);
				$mtime = fstat($fp);
				$mtime = $mtime['mtime'];
				if (time() - $mtime < $tmp) $fatal = false;
			}
			fclose($fp);
			if ($fatal) {
				FatalError('Cant lock tmp lock file: '.$job['jobname']);
			}
			# else: Another instance of job is running and it's OK
			exit();
		}
		touch($fname); # update mtime

		$tmStart = time();
		// !!! run code here !!!
		$db   = new PXDatabase($app); //add NEW connect to the database from current child
		$user = new PXUserCron();
		$user->setDb($db);
		$db->LoadDirectoriesAutomatic($app->directory); //init directory from here !
		if (isset($app->types['struct'])) {
			$tree = new NLTree($db->getObjects($app->types['struct'], true));
		} else {
			$tree = null;
		}

		$res = $job['job']->Run($app, $db, $tree, $matchedTime, $job['rule']);

		// !!! run code here !!!
		$tmEnd   = time();
		flock($fp, LOCK_UN);
		fclose($fp);

		// log here
		// log  up

		$cronStat = array();
		$fp = @fopen(BASEPATH.'/var/cron.results', "r+");
		if ($fp) {
			do{
				//nothing;
			} while(!flock($fp, LOCK_EX));
			fseek($fp, 0, SEEK_END);
			$fsize = ftell($fp);
			rewind($fp);
			if ($fsize > 0) {
				$tStat = unserialize(fread($fp, $fsize));
				ftruncate($fp, 0);
				rewind($fp);
				foreach ($tStat as $tHash => $vHash) {
					if (isset($this->job2rule[$tHash])) {
						$cronStat[$tHash] = $vHash;
					}
				}
			}
		} else {
			$fp = @fopen(BASEPATH.'/var/cron.results', "w");
			if($fp){
				do{
					//nothing;
				} while(!flock($fp, LOCK_EX));
			}
		}
		$cronStat[$job['jobhash']] = array('start' => $tmStart, 'end' => $tmEnd, 'result' => $res);
		fwrite($fp, serialize($cronStat));
		flock($fp, LOCK_UN);
		fclose($fp);
	}

	function RunTasks(&$app, $matchedTime) {
		$t = localtime($matchedTime, true);
		$pid = -1;

		foreach ($this->rules as $k) {
			if ($this->isNotMatchTime($k['rule']->match, $t)) {
				continue;
			}
			if (IS_WIN || ($pid = pcntl_fork()) == 0) {
				$this->RunJob($k, $app, $matchedTime); //child code
				if(!IS_WIN) exit(); //close child process
			}
		}

		if(!IS_WIN && $pid) { //parent code
			while(pcntl_waitpid(-1, $status) > 0){
				//wanna some status ?
			}
		}
	}
	
	function isNotMatchTime($match, $t){
		return !(
			isset($match['min'][$t['tm_min']])   &&
			isset($match['hour'][$t['tm_hour']]) &&
			isset($match['mday'][$t['tm_mday']]) &&
			isset($match['mon'][$t['tm_mon']+1]) &&
			isset($match['wday'][$t['tm_wday']])
		);
	}

	function adminIndex() {
		$fp = @fopen(BASEPATH.'/var/cron.results', 'r');
		if ($fp) {
			flock($fp, LOCK_EX);

			fseek($fp, 0, SEEK_END); $fsize = ftell($fp); rewind($fp);
			$cronStat = unserialize(fread($fp, $fsize));

			flock($fp, LOCK_UN);
			fclose($fp);
		} else {
			$cronStat = array();
		}
		$this->layout->SetInnerLayout(
			array(
				array(array('100%', '100%')),
				array(array('100%', NULL))
			)
		);
		$this->layout->Assign('INNER.0.1', '<a href="?area='.$this->area.'&t='.time().'" class="reload">Обновить</a>');

		$fields = array(
			'rule'    => 'Правило',
			'name'    => 'Название задачи',
			'time'    => 'Начало/Окончание',
			'comment' => 'Примечание',
		);

		$result = array();
		foreach ($this->rules as $k) {
			$_ = array();

			$_['rule'] = $k['rule']->asString;
			$_['name'] = $k['job']->name;

			if (isset($cronStat[$k['jobhash']])) {
				$t = $cronStat[$k['jobhash']];
			} else {
				$t = NULL;
			}

			if (!is_null($t)) {
				$diff = $t['end']-$t['start'];
				$dmin = (int) ($diff / 60);
				$dsec =  $diff - $dmin*60;

				$_['time'] = date($this->TIME_FORMAT, $t['start']).'<br>'.date($this->TIME_FORMAT, $t['end']).' ('.$dmin.' min '.$dsec.' sec)';

				$_['comment'] = ($t['result']['status'] >= 0) ? '<b>'.$t['result']['note'].'</b>' : '<b class="error">'.$t['result']['note'].'</b>';

			} else {
				$_['time'] = '&nbsp;<br>&nbsp;';
				$_['comment'] = 'Еще не запускалась';
			}

			$result[] = $_;
		}

		$table = new PXAdminTableSimple($fields);
		$table->setData($result);

		$this->layout->Assign('INNER.0.0', $table->html());
	}
}
?>
