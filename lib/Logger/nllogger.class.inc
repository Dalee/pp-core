<?php
/**
 * Project:     Dalee PHP-classes library
 * File:        nllogger.classes.inc
 *
 * @copyright   Dalee, 2003-2010
 * @author      Andrey Nikolaev <na@index20.ru>
 * @package     ProximaPortal
 * @subpackage  Logger
 */

/**
 * NLLogger class.
 *
 * @todo 1. FileAppender - добавить максимальный размер файла, продумать ротацию логов --  mostly DONE
 * @todo 2. Дотестировать SyslogAppender, привести в порядок map, facilitymap, defines
 * @todo 3. Документация ?  --  DONE
 * @todo 4. MailAppender - отправка логов по email
 * @todo 5. HTMLAppender ???
 */
class NLLogger
{

    public $appenders = [];

    public $level = 6; //Default - NONE

    public $ALL = 0;
    public $DEBUG = 1;
    public $INFO = 2;
    public $WARN = 3;
    public $ERROR = 4;
    public $FATAL = 5;
    public $NONE = 6;

    public $parent = null; //parent logger uplink

    public function __construct(public $name)
    {
    }

    public static function getLogger($name)
    {
        static $instances;

        if (!isset($instances)) {
            $instances = [];
        }

        if (!isset($instances[$name])) {
            $instances[$name] = new NLLogger($name);
        }

        //setup parent logger uplink
        if (str_contains((string) $name, '.')) {
            $tmp = explode('.', strrev((string) $name), 2);
            if ($tmp[1] !== '') {
                $tmp_name = strrev($tmp[1]);
                $instances[$name]->parent = NLLogger::getLogger($tmp_name);
            }
        }

        return $instances[$name];
    }

    //appenders "Factory"
    public function addAppender($name, $class)
    {
        $ret = false;
        if (class_exists($class)) {
            $tmp = new $class($name);
            if (is_subclass_of($tmp, "Appender")) {
                $this->appenders[$name] = $tmp;
                $ret = true;
            }
        }
        return $ret;
    }

    public function setAppenderParam($name, $param, $value)
    {
        $ret = false;
        if (isset($this->appenders[$name])) {
            $ret = $this->appenders[$name]->setParam($param, $value);
        }
        return $ret;
    }

    public function setAppenderParamByRef($name, $param, &$value)
    {
        $ret = false;
        if (isset($this->appenders[$name])) {
            $ret = $this->appenders[$name]->setParamByRef($param, $value);
        }
        return $ret;
    }

    //usage: $myLogger->setLevel($myLogger->DEBUG)
    public function setLevel($level)
    {
        $this->level = $level;
    }

    public function getLevel()
    {
        return $this->level;
    }

    public function log($level, $message)
    {
        if ($level >= $this->level) {
            foreach ($this->appenders as $appender) {
                if ($appender->isEnabled()) {
                    $appender->write($level, $message);
                }
            }
        }
        if ($this->parent !== null) {
            $this->parent->log($level, $message);
        }
    }

    //shortcurts to log(level, msg):
    public function debug($message)
    {
        $this->log($this->DEBUG, $message);
    }

    public function info($message)
    {
        $this->log($this->INFO, $message);
    }

    public function warn($message)
    {
        $this->log($this->WARN, $message);
    }

    public function error($message)
    {
        $this->log($this->ERROR, $message);
    }

    public function fatal($message)
    {
        $this->log($this->FATAL, $message);
    }
}

/**
 * Appender Abstract class.
 *
 */
class Appender
{
    public $enabled = true;
    public $format = '[%t] %1 ';

    //constructor
    public function __construct(public $name)
    {
    }

    //must be redefined in child class
    public function write($level, $message)
    {
    }

    public function isEnabled()
    {
        return $this->enabled;
    }

    public function setParam($param, $value)
    {
        $ret = false;
        $classVars = get_class_vars(static::class);
        if (array_key_exists($param, $classVars)) {
            $this->$param = $value;
            $ret = true;
        }
        return $ret;
    }

    public function setParamByRef($param, &$value)
    {
        $ret = false;
        $classVars = get_class_vars(static::class);
        if (array_key_exists($param, $classVars)) {
            $this->$param = &$value;
            $ret = true;
        }
        return $ret;
    }

    public function _compileMessage($level, $message)
    {
        $pattern = ['%n', '%t', '%l', '%r'];
        $replace = [$this->name, date('Y-m-d H:i:s'), $level, random_int(1, 10000)];
        if (is_array($message)) {
            $i = 1;
            foreach ($message as $part) {
                $pattern[] = '%' . $i++;
                $replace[] = $part;
            }
        } else {
            $pattern[] = '%1';
            $replace[] = $message;
        }
        return str_replace($pattern, $replace, (string) $this->format);
    }
}

/**
 * ConsoleAppender class.
 *
 */
class ConsoleAppender extends Appender
{
    public function write($level, $message)
    {
        echo $this->_compileMessage($level, $message) . "\n";
    }
}

/**
 * FileAppender class.
 *
 */
class FileAppender extends Appender
{
    public $writeMode = 'a+';
    public $fileSize = '2097152'; //2 Mb default
    public $fileName;

    public function write($level, $message)
    {
        if (isset($this->fileName)) {
            $fd = $this->_getFile($this->fileName);
            if ($fd !== false) {
                $msg = $this->_compileMessage($level, $message) . "\n";
                @fwrite($fd, $msg);
            } else {
                trigger_error("Logger error - can't open file: {$this->fileName}", E_USER_ERROR);
            }
        } else {
            trigger_error('Logger error - FileAppender->fileName should not be empty', E_USER_ERROR);
        }
    }

    public function _getFile($fileName)
    {

        static $openFiles;

        if (!isset($openFiles)) {
            $openFiles = [];
        }

        if (isset($openFiles[$fileName]) && file_exists($fileName) && filesize($fileName) >= $this->fileSize) {
            fclose($openFiles[$fileName]);
            $rotateFileName = $fileName . '.' . time();
            //paranoid_mode = on
            if (file_exists($rotateFileName))
                $rotateFileName .= '.' . md5(random_int(0, mt_getrandmax()));
            rename($fileName, $rotateFileName);
            unset($openFiles[$fileName]);
        }

        if (!isset($openFiles[$fileName])) {
            $openFiles[$fileName] = @fopen($fileName, $this->writeMode);
        }

        return $openFiles[$fileName];
    }
}

/**
 * SQLAppender class.
 *
 */
class SQLAppender extends Appender
{
    /** @var PXDatabase */
    public $db;

    public function __construct($name)
    {
        parent::__construct($name);
        $this->format = "";
    }

    public function write($level, $message)
    {
        if ($this->db !== null && $this->format) {
            // Escape text message
            $msg = $this->_compileMessage(
                $level,
                array_map($this->db->EscapeString(...), (array)$message)
            );

            //4th argument is false -- do not flush cache !
            $r = $this->db->ModifyingQuery($msg, null, null, false);

            if ($r < 0) {
                trigger_error('Logger error - can\'t write to database', E_USER_ERROR);
            }
        }
    }
}

/**
 * SyslogAppender class.
 *
 */
class SyslogAppender extends Appender
{
    public $map = [];
    public $ident = 'nllogger';
    public $facility = 'LOG_USER';
    public $facilitymap = null;

    public function __construct($name)
    {
        parent::__construct($name);

        $this->format = '%1';
        $this->map = [
            '1' => LOG_DEBUG,
            '2' => LOG_INFO,
            '3' => LOG_WARNING,
            '4' => LOG_ERR,
            '5' => LOG_CRIT //mb 5=>'LOG_ERR' ?
        ];

        $this->facilitymap = [
            'LOG_MAIL' => LOG_MAIL, 'LOG_DAEMON' => LOG_DAEMON,
            'LOG_AUTH' => LOG_AUTH, 'LOG_SYSLOG' => LOG_SYSLOG,
            'LOG_LPR' => LOG_LPR, 'LOG_NEWS' => LOG_NEWS,
            'LOG_UUCP' => LOG_UUCP, 'LOG_CRON' => LOG_CRON,
            'LOG_AUTHPRIV' => LOG_AUTHPRIV, 'LOG_LOCAL0' => LOG_LOCAL0,
            'LOG_LOCAL1' => LOG_LOCAL1, 'LOG_LOCAL2' => LOG_LOCAL2,
            'LOG_LOCAL3' => LOG_LOCAL3, 'LOG_LOCAL4' => LOG_LOCAL4,
            'LOG_LOCAL5' => LOG_LOCAL5, 'LOG_LOCAL6' => LOG_LOCAL6,
            'LOG_LOCAL7' => LOG_LOCAL7, 'LOG_USER' => LOG_USER,
        ];
    }

    public function write($level, $message)
    {
        static $facilities;

        if ($facilities === null) {
            $facilities = [];
        }

        if (!isset($facilities[$this->facilitymap[$this->facility]])) {
            $facilities[$this->facilitymap[$this->facility]] = openlog($this->ident, LOG_PID, $this->facilitymap[$this->facility]);
        }

        syslog($this->map[$level], $this->_compileMessage($level, $message));
    }
}
