<?php

namespace PP\Module;

use PP\Cron\AbstractCron;
use PP\DependencyInjection\ContainerAwareInterface;
use PP\Lib\Datastruct\Tree;
use PP\Cron\CronRule;
use PXRegistry;
use PXUserCron;
use PXDatabase;
use PXAdminTableSimple;

/**
 * Class CronRunModule.
 *
 * @package PP\Module
 */
class CronRunModule extends AbstractModule
{
    protected $timeFormat = 'd-m-Y H:i:s';

    /** @var array */
    public $rules;

    /** @var array */
    public $job2rule;

    /** @var array */
    public $jobs;

    /** @var string */
    protected $resultsFile;

    /** @var string */
    protected $tempDir;

    public function __construct($area, $settings)
    {
        parent::__construct($area, $settings);

        $this->resultsFile = RUNTIME_PATH . 'cron.results';
        $this->tempDir = RUNTIME_PATH . 'lock' . DIRECTORY_SEPARATOR . 'cronrun';
        $this->rules = [];
        $this->job2rule = [];
        $this->jobs = [];

        $this->_parseRules($settings);
    }

    public function _parseRules($settings)
    {
        if (!isset($settings['rule']) || !(is_countable($settings['rule']) ? count($settings['rule']) : 0)) {
            return;
        }

        $count = 0;
        foreach ($settings['rule'] as $s) {
            if (!preg_match("/^\s*(.+?)\s+(\w+?)\s*$/", (string) $s, $m)) {
                continue;
            }

            $rule = new CronRule($m[1]);
            $jobName = $m[2];

            if (!$rule->valid) {
                continue;
            }

            $file = strtolower($jobName) . '.cronrun.inc';

            if (!class_exists(sprintf("pxcronrun%s", strtolower($jobName)), false)) {
                if (file_exists(BASEPATH . '/local/cronruns/' . $file)) {
                    include_once BASEPATH . '/local/cronruns/' . $file;
                } elseif (file_exists(BASEPATH . '/libpp/cronruns/' . $file)) {
                    include_once BASEPATH . '/libpp/cronruns/' . $file;
                } else {
                    continue;
                }
            }

            $class = 'PXCronRun' . $jobName;
            $instance = new $class();

            $rm = [
                'rule' => $rule,
                'jobhash' => $rule->matchHash . md5($jobName),
                'jobname' => strtolower($jobName),
                'job' => $instance,
            ];

            $this->rules[$count] = $rm;
            $this->jobs[$jobName] = &$this->rules[$count];
            $this->job2rule[$rm['jobhash']] = $count;
            $count++;
        }
    }

    public function runJob(&$job, &$app, $matchedTime)
    {
        MakeDirIfNotExists($this->tempDir);

        // ******* child code ******
        $fname = $this->tempDir . DIRECTORY_SEPARATOR . $job['jobname'] . '.lock';
        $fp = @fopen($fname, 'r');

        if (!$fp) {
            $fp = fopen($fname, 'w');
        }

        $st = flock($fp, LOCK_EX | LOCK_NB);
        if (!$st) {
            $fatal = true;

            if (isset($job['job']->longrunner)) {
                $tmp = intval($job['job']->longrunner);
                $mtime = fstat($fp);
                $mtime = $mtime['mtime'];

                if (time() - $mtime < $tmp) {
                    $fatal = false;
                }
            }

            flock($fp, LOCK_UN);
            fclose($fp);

            if ($fatal) {
                FatalError('Cant lock tmp lock file: ' . $job['jobname']);
            }
            # else: Another instance of job is running and it's OK
            exit();
        }

        touch($fname); # update mtime
        $tmStart = time();

        // !!! run code here !!!
        // add NEW connect to the database from current child
        $db = new PXDatabase($app);
        $user = new PXUserCron();
        $db->setUser($user);

        PXRegistry::setDB($db);
        PXRegistry::setUser($user);

        $db->loadDirectoriesAutomatic($app->directory);
        if (isset($app->types[DT_STRUCT])) {
            $tree = new Tree($db->getObjects($app->types[DT_STRUCT], true));
        } else {
            $tree = null;
        }

        if ($job['job'] instanceof ContainerAwareInterface) {
            $job['job']->setContainer($this->container);
        }

        /** @var AbstractCron $jobInstance */
        $jobInstance = $job['job'];
        $res = $jobInstance->Run($app, $db, $tree, $matchedTime, $job['rule']);

        // !!! run code here !!!
        $tmEnd = time();
        flock($fp, LOCK_UN);
        fclose($fp);

        $cronStat = [];
        $fp = @fopen($this->resultsFile, "r+");

        if ($fp) {
            do {
                //nothing;
            } while (!flock($fp, LOCK_EX));

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
            $fp = @fopen($this->resultsFile, "w");

            if ($fp) {
                do {
                    // nothing;
                } while (!flock($fp, LOCK_EX));
            }
        }

        $cronStat[$job['jobhash']] = ['start' => $tmStart, 'end' => $tmEnd, 'result' => $res];

        fwrite($fp, serialize($cronStat));
        flock($fp, LOCK_UN);
        fclose($fp);
    }

    public function RunTasks(&$app, $matchedTime)
    {
        $t = localtime($matchedTime, true);
        $pid = -1;

        foreach ($this->rules as $k) {
            if ($this->isNotMatchTime($k['rule']->match, $t)) {
                continue;
            }

            if (($pid = pcntl_fork()) == 0) {
                $this->runJob($k, $app, $matchedTime); //child code
                exit(); //close child process
            }
        }

        if ($pid) { //parent code
            while (pcntl_waitpid(-1, $status) > 0) {
                //wanna some status ?
            }
        }
    }

    public function isNotMatchTime($match, $t)
    {
        return !(
            isset($match['min'][$t['tm_min']]) &&
            isset($match['hour'][$t['tm_hour']]) &&
            isset($match['mday'][$t['tm_mday']]) &&
            isset($match['mon'][$t['tm_mon'] + 1]) &&
            isset($match['wday'][$t['tm_wday']])
        );
    }

    private function _loadStat()
    {
        $fp = @fopen($this->resultsFile, 'r');

        if ($fp) {
            flock($fp, LOCK_EX);

            fseek($fp, 0, SEEK_END);
            $fsize = ftell($fp);
            rewind($fp);
            $cronStat = unserialize(fread($fp, $fsize));

            flock($fp, LOCK_UN);
            fclose($fp);

        } else {
            $cronStat = [];
        }

        return $cronStat;
    }

    public function getStat($job)
    {
        static $cronStat;

        if (is_null($cronStat)) {
            $cronStat = $this->_loadStat();
        }

        if (isset($cronStat[$job['jobhash']])) {
            $t = $cronStat[$job['jobhash']];
        } else {
            $t = null;
        }

        return $t;
    }

    public function adminIndex()
    {
        $layout = $this->layout;

        $layout->setOneColumn();
        $layout->assign('INNER.0.1', '<a href="?area=' . $this->area . '&t=' . time() . '" class="reload">Обновить</a>');

        $fields = [
            'rule' => 'Правило',
            'name' => 'Название задачи',
            'time' => 'Начало/Окончание',
            'comment' => 'Примечание',
        ];

        $result = [];

        foreach ($this->rules as $k) {
            $_ = [];

            $_['rule'] = $k['rule']->asString;
            $_['name'] = $k['job']->name;


            $t = $this->getStat($k);

            if (!is_null($t)) {
                $diff = $t['end'] - $t['start'];
                $dmin = (int)($diff / 60);
                $dsec = $diff - $dmin * 60;

                $_['time'] = date($this->timeFormat, $t['start']) . '<br>' . date($this->timeFormat, $t['end']) . ' (' . $dmin . ' min ' . $dsec . ' sec)';
                $note = isset($t['result']['note']) ? htmlentities((string) $t['result']['note'], ENT_COMPAT | ENT_HTML401, DEFAULT_CHARSET) : '';
                $_['comment'] = ($t['result']['status'] >= 0) ? '<b>' . $note . '</b>' : '<b class="error">' . $note . '</b>';

            } else {
                $_['time'] = '&nbsp;<br>&nbsp;';
                $_['comment'] = 'Еще не запускалась';
            }

            $result[] = $_;
        }

        $table = new PXAdminTableSimple($fields);
        $table->setData($result);

        $layout->assign('INNER.0.0', $table->html());
    }
}
