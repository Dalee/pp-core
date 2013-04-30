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
class NLLogger {

	var $appenders = array();
	var $name  = '';

	var $level = 6; //Default - NONE

	var $ALL   = 0;
	var $DEBUG = 1;
	var $INFO  = 2;
	var $WARN  = 3;
	var $ERROR = 4;
	var $FATAL = 5;
	var $NONE  = 6;

	var $parent = null; //parent logger uplink

	function NLLogger($name) {
		$this->name = $name;
	}

	static function getLogger($name) {
		static $instances;

		if (!isset($instances)) {
			$instances = array();
		}

		if (!isset($instances[$name])) {
			$instances[$name] = new NLLogger($name);
		}

		//setup parent logger uplink
		if (strpos($name, '.') !== FALSE) {
			$tmp = explode('.',strrev($name), 2);
			if ($tmp[1] !== '') {
				$tmp_name = strrev($tmp[1]);
				$instances[$name]->parent = NLLogger::getLogger($tmp_name);
			}
		}

		return $instances[$name];
	}

	//appenders "Factory"
	function addAppender($name, $class) {
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

	function setAppenderParam($name, $param, $value){
		$ret = false;
		if (isset($this->appenders[$name])) {
			$ret = $this->appenders[$name]->setParam($param, $value);
		}
		return $ret;
	}

	function setAppenderParamByRef($name, $param, &$value){
		$ret = false;
		if (isset($this->appenders[$name])) {
			$ret = $this->appenders[$name]->setParamByRef($param, $value);
		}
		return $ret;
	}

	//usage: $myLogger->setLevel($myLogger->DEBUG)
	function setLevel($level){
		$this->level = $level;
	}
	
	function getLevel(){
		return $this->level;
	}

	function log($level, $message) {
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
	function debug($message) {
		$this->log($this->DEBUG, $message);
	}
	function info($message)  {
		$this->log($this->INFO, $message);
	}
	function warn($message)  {
		$this->log($this->WARN, $message);
	}
	function error($message) {
		$this->log($this->ERROR, $message);
	}
	function fatal($message) {
		$this->log($this->FATAL, $message);
	}
}

/**
 * Appender Abstract class.
 *
 */
class Appender {
	var $name    = '';
	var $enabled = true;
	var $format  = '[%t] %1 ';

	//constructor
	function Appender($name) {
		$this->name = $name;
	}

	//must be redefined in child class
	function write($level, $message) {}

	function isEnabled() {
		return $this->enabled;
	}

	function setParam($param, $value){
		$ret = false;
		$classVars = get_class_vars(get_class($this));
		if (array_key_exists($param, $classVars)){
			$this->$param = $value;
			$ret = true;
		}
		return $ret;
	}

	function setParamByRef($param, &$value){
		$ret = false;
		$classVars = get_class_vars(get_class($this));
		if (array_key_exists($param, $classVars)){
			$this->$param = &$value;
			$ret = true;
		}
		return $ret;
	}

	function _compileMessage ($level, $message) {
		$pattern = array('%n', '%t', '%l', '%r' );
		$replace = array($this->name, date('Y-m-d H:i:s'), $level, rand(1, 10000));
		if (is_array($message)) {
			$i = 1;
			foreach ($message as $part) {
				$pattern[] = '%'.$i++;
				$replace[] = $part;
			}
		} else {
			$pattern[] = '%1';
			$replace[] = $message;
		}
		return str_replace($pattern, $replace, $this->format);
	}
}

/**
 * ConsoleAppender class.
 *
 */
class ConsoleAppender extends Appender {
	function write($level, $message) {
		echo $this->_compileMessage($level, $message) . "\n";
	}
}

/**
 * FileAppender class.
 *
 */
class FileAppender extends Appender {
	var $writeMode = 'a+';
	var $fileSize  = '2097152'; //2 Mb default
	var $fileName;

	function write($level, $message) {
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

	function _getFile($fileName) {

		static $openFiles;

		if (!isset($openFiles)) {
			$openFiles = array();
		}

		if (isset($openFiles[$fileName]) && file_exists($fileName) && filesize($fileName) >= $this->fileSize ) {
			fclose($openFiles[$fileName]);
			$rotateFileName = $fileName .'.'. mktime();
			//paranoid_mode = on
			if (file_exists($rotateFileName))
				$rotateFileName .= '.' . md5(mt_rand());
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
class SQLAppender extends Appender {
	var $db;

	function SQLAppender($name) {
		parent::Appender($name);
		$this->format = "";
	}

	function write($level, $message) {
		if ($this->db !== null && $this->format){
			// Escape text message
			$msg = $this->_compileMessage(
				$level,
				array_map(array($this->db, 'EscapeString'), (array)$message)
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
class SyslogAppender extends Appender{
	var $map         = array();
	var $ident       = 'nllogger';
	var $facility    = 'LOG_USER';
	var $facilitymap = null;

	function SyslogAppender($name) {
		parent::Appender($name);
		if (!defined('LOG_PID')) {
			define_syslog_variables();
		}
		$this->format = '%1';
		$this->map    = array(
			'1' => LOG_DEBUG,
			'2' => LOG_INFO,
			'3' => LOG_WARNING,
			'4' => LOG_ERR,
			'5' => LOG_CRIT //mb 5=>'LOG_ERR' ?
		);

		$this->facilitymap = array(
			'LOG_MAIL'     => LOG_MAIL,     'LOG_DAEMON' => LOG_DAEMON,
			'LOG_AUTH'     => LOG_AUTH,     'LOG_SYSLOG' => LOG_SYSLOG,
			'LOG_LPR'      => LOG_LPR,      'LOG_NEWS'   => LOG_NEWS,
			'LOG_UUCP'     => LOG_UUCP,     'LOG_CRON'   => LOG_CRON,
			'LOG_AUTHPRIV' => LOG_AUTHPRIV, 'LOG_LOCAL0' => LOG_LOCAL0,
			'LOG_LOCAL1'   => LOG_LOCAL1,   'LOG_LOCAL2' => LOG_LOCAL2,
			'LOG_LOCAL3'   => LOG_LOCAL3,   'LOG_LOCAL4' => LOG_LOCAL4,
			'LOG_LOCAL5'   => LOG_LOCAL5,   'LOG_LOCAL6' => LOG_LOCAL6,
			'LOG_LOCAL7'   => LOG_LOCAL7,   'LOG_USER'   => LOG_USER
		);
	}

	function write ($level, $message) {
		static $facilities;

		if ($facilities === null) {
			$facilities = array();
		}

		if ( !isset($facilities[$this->facilitymap[$this->facility]]) ) {
			$facilities[$this->facilitymap[$this->facility]] = openlog($this->ident, LOG_PID, $this->facilitymap[$this->facility]);
		}

		syslog($this->map[$level], $this->_compileMessage($level, $message));
	}
}
?>
