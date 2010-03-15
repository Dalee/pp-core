<?php
/**
 * Класс - одиночка "Singleton" для журналирования событий в ProximaPortal.
 */

class PXAuditLogger {
	/**
	 * Ссылка на PXApplication
	 * @var PXApplication
	 */
	var $app;

	/**
	 * Ссылка на PXDatabase
	 * @var PXDatabase
	 */
	var $db;

	/**
	 * Ссылка на NLLogger
	 * @var NLLogger
	 */
	var $logger;
	
	var $logLevel;

	/**
	 * Конструктор класса.
	 *
	 * Класс PXAuditLogger является singleton оболочкой класса {@link NLLogger}.
	 * Создание экзепляров класса должно производится через метод {@link PXAuditLogger::getLogger()}.
	 *
	 * @param PXApplication $app ссылка на PXApplication {@link $app}
	 * @param PXDatabase $db ссылка на PXDatabase {@link $db}
	 */
	function PXAuditLogger($app, $db) {
		$this->app = $app;
		$this->db  = $db;

		$this->logger = NLLogger::getLogger('audit');
		$this->logger->addAppender('sql', 'SQLAppender');

		if ($this->app->dbDescription['database']->dbtype == 'mssql') {
			$this->logger->setAppenderParam('sql', 'format', 'SET DATEFORMAT ymd; INSERT INTO log_audit ([ts], [level], [type], '.
			' [source], [user], [ip], [description]) '.
			" VALUES ('%t', '%l', '%1', '%2', '%3', '%4', '%5'); SET DATEFORMAT dmy;");

		} else {
			$this->logger->setAppenderParam('sql', 'format', 'INSERT INTO log_audit ("ts", "level", "type", '.
			' "source", "user", "ip", "description") '.
			" VALUES ('%t', '%l', '%1', '%2', '%3', '%4', '%5') ");
		}
		$this->logLevel = $this->logger->INFO;
		$this->logger->setLevel($this->logLevel);
		$this->logger->setAppenderParamByRef('sql', 'db', $this->db);
	}

	/**
	 * Конструктор - синглетон.
	 *
	 * @param PXApplication $app ссылка на PXApplication {@link $app}
	 * @param PXDatabase $db ссылка на PXDatabase {@link $db}
	 * @return PXAuditLogger - single instance of PXauditLogger.
	 */

	static function &getLogger() {
		static $instance;

		if ($instance === null) {
			$app = & PXRegistry::getApp();
			$db  = & PXRegistry::getDb();

			$class = $app->getProperty('LOGGER_CLASS', 'PXAuditLogger');
			$instance = new $class($app, $db);
		}

		return $instance;
	}

	/**
	 * Метод оболочка для вызова {@link NLLogger::debug()}.
	 * Использует {@link PXAuditLogger::_createMsg()} для формирования сообщения.
	 *
	 * @param String $description - текстовое сообщение
	 * @param String $source - источник сообщения (обычно datatype/id)
	 * @param String $type - текстовый тип сообщения AUDIT, USER, итп.
	 */
	function debug($description, $source = 'UNKNOWN', $type = 'AUDIT') {
		$this->logger->debug($this->_createMsg($description, $source, $type));
	}
	/**
	 * Метод оболочка для вызова {@link NLLogger::info()}.
	 * Параметры описаны в {@link PXAuditLogger::debug()}
	 */
	function info($description, $source = 'UNKNOWN', $type = 'AUDIT') {
		$this->logger->info($this->_createMsg($description, $source, $type));
	}
	/**
	 * Метод оболочка для вызова {@link NLLogger::warn()}.
	 * Параметры описаны в {@link PXAuditLogger::debug()}
	 */
	function warn($description, $source = 'UNKNOWN', $type = 'AUDIT') {
		$this->logger->warn($this->_createMsg($description, $source, $type));
	}
	/**
	 * Метод оболочка для вызова {@link NLLogger::error()}.
	 * Параметры описаны в {@link PXAuditLogger::debug()}
	 */
	function error($description, $source = 'UNKNOWN', $type = 'AUDIT') {
		$this->logger->error($this->_createMsg($description, $source, $type));
	}
	/**
	 * Метод оболочка для вызова {@link NLLogger::fatal()}.
	 * Параметры описаны в {@link PXAuditLogger::debug()}
	 */
	function fatal($description, $source = 'UNKNOWN', $type = 'AUDIT') {
		$this->logger->fatal($this->_createMsg($description, $source, $type));
	}
	/**
	 * Формирует сообщение для записи в лог.
	 *
	 * @param String $description - текстовое сообщение
	 * @param String $source - источник сообщения (обычно datatype/id)
	 * @param String $type - текстовый тип сообщения AUDIT, USER, итп.
	 * @return Array - массив из 5 элементов для передачи в NLLogger->debug|...|fatal()
	 */
	function _createMsg($description, $source, $type) {
		$user = $this->db->user->isAuthed() ? $this->db->user->login : 'UNKNOWN';
		$ip   = (PXRequest::GetRemoteAddr() !== null) ? PXRequest::GetRemoteAddr() : 'UNKNOWN';
		return array($type, $source, $user, $ip, $description);
	}
	
	/**
	 * Static метод, включение логгера
	 */
	 function on(){
	 	if(PXAuditLogger::disabled()){
	 		$self = & PXAuditLogger::getLogger();
	 		$self->logger->setLevel($self->logLevel);
	 	}
	 }
	
	/**
	 * Static метод, отключение логгера
	 */
	 function off(){
	 	$self = & PXAuditLogger::getLogger();
	 	$self->logger->setLevel($self->logger->NONE);
	 }
	 
	 /**
	 * Static метод, возвращает true если лог не ведётся
	 */
	 function disabled(){
	 	$self = & PXAuditLogger::getLogger();
	 	return $self->logger->NONE == $self->logger->getLevel();
	 }
}
?>
