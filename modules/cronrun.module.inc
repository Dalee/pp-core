<?

define('TIME_FORMAT',   "d-m-Y H:i:s");

class PXModuleCronRun extends PXAbstractModule {
	var $rules;

	function PXModuleCronRun($area, $settings) {
		$this->PXAbstractModule($area, $settings);

		$this->results  = BASEPATH.'/site/var/cron.results';

		$this->rules    = array();
		$this->job2rule = array();
		$this->jobs     = array();

		if (!isset($settings['rule']) ||
			!count($settings['rule'])) return;

		if (count($settings['rule']) == 1) {
			$settings['rule'] = array($settings['rule']);
		}

		$count = 0;
		foreach ($settings['rule'] as $s) {
			if (!preg_match("/^\s*(.+?)\s+(\w+?)\s*$/", $s, $m)) continue;
			$rule     = new PXCronRule($m[1]);
			$jobname  = $m[2];
			if (!$rule->valid) continue;
			$file = strtolower($jobname).".cronrun.inc";

			if (file_exists(BASEPATH.'/local/cronruns/'.$file)) {
				require_once BASEPATH.'/local/cronruns/'.$file;
			} elseif (file_exists(BASEPATH.'/libpp/cronruns/'.$file)) {
				require_once BASEPATH.'/libpp/cronruns/'.$file;
			} else {
				continue;
			}

			$class = 'PXCronRun' . $jobname;
			$rm = array('rule'    => $rule,
						'jobhash' => $rule->matchHash.md5($jobname),
						'jobname' => strtolower($jobname),
						'job'     => new $class());

			$this->rules[$count] = $rm;
			$this->jobs[$jobname] = &$this->rules[$count];
			$this->job2rule[$rm['jobhash']] = $count;
			$count++;
		}

	}

	function RunJob(&$job, &$app, $matchedTime) {
		$tmpDir = BASEPATH . DIRECTORY_SEPARATOR . 'tmp' . DIRECTORY_SEPARATOR;

		// ******* child code ******
		$fname = $tmpDir.'cron.'.$job['jobname'].'.lock';
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

		$db = new PXDatabase($app);
		$user = new PXUserCron();
		$user->setDb($db);
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
		$fp = @fopen($this->results, "r+");
		if ($fp) {
			flock($fp, LOCK_EX);
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
			$fp = @fopen($this->results, "w");
			flock($fp, LOCK_EX);
		}
		$cronStat[$job['jobhash']] = array('start' => $tmStart, 'end' => $tmEnd, 'result' => $res);
		fwrite($fp, serialize($cronStat));
		flock($fp, LOCK_UN);
		fclose($fp);
	}

	function RunTasks(&$app, $matchedTime) {
		$pids = array();
		$t = localtime($matchedTime, true);
		foreach ($this->rules as $k) {
			$match = $k['rule']->match;
			if (!(
				isset($match['min'][$t['tm_min']])   && isset($match['hour'][$t['tm_hour']]) &&
				isset($match['mday'][$t['tm_mday']]) && isset($match['mon'][$t['tm_mon']+1]) &&
				isset($match['wday'][$t['tm_wday']])
				)) continue;

			if (!IS_WIN && ($pid = pcntl_fork()) > 0) {
				$pids[] = $pid;
				continue;
			}

			$this->RunJob($k, $app, $matchedTime);

			if(!IS_WIN) {
				exit();
			}
			// ******* child code ******
		}

		if(!IS_WIN) {
			foreach ($pids as $pid) {
				pcntl_waitpid($pid, $status);
			}
//			while(pcntl_wait($status) != -1) {} // PHP5 only :(
		}
	}

	function adminIndex(&$app, &$request, &$user, &$db, &$layout) {
		$fp = @fopen($this->results, 'r');
		if ($fp) {
			flock($fp, LOCK_EX);

			fseek($fp, 0, SEEK_END); $fsize = ftell($fp); rewind($fp);
			$cronStat = unserialize(fread($fp, $fsize));

			flock($fp, LOCK_UN);
			fclose($fp);
		} else {
			$cronStat = array();
		}
		$layout->SetInnerLayout(
			array(
				array(array('100%', '100%')),
				array(array('100%', NULL))
			)
		);
		$layout->Assign('INNER.0.1', '<DIV style="text-align: center"><A href="?area='.$request->GetArea('cronrun').'&t='.time().'" class="button"><IMG src="i/icon/on.gif" border="0" width="11" height="11" alt="Обновить" align="absmiddle" hspace="3">Обновить</A></DIV>');

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

				$_['time'] = date(TIME_FORMAT, $t['start']).'<br>'.date(TIME_FORMAT, $t['end']).' ('.$dmin.' min '.$dsec.' sec)';

				$_['comment'] = ($t['result']['status'] >= 0) ? '<b>'.$t['result']['note'].'</b>' : '<b class="error">'.$t['result']['note'].'</b>';

			} else {
				$_['time'] = '&nbsp;<br>&nbsp;';
				$_['comment'] = 'Еще не запускалась';
			}

			$result[] = $_;
		}

		$table = new PXAdminTableSimple($fields);
		$table->setData($result);

		$layout->Assign('INNER.0.0', $table->html());
	}

}

?>
