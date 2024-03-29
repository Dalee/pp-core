<?php
/**
 * Класс - одиночка "Singleton" для журналирования событий в ProximaPortal.
 */

// TODO: refactor to use monolog
class PXAuditLogger
{
	/**
	 * Ссылка на NLLogger
	 * @var NLLogger
	 */
	public $logger;

	public $logLevel;

	/**
	 * Конструктор класса.
	 *
	 * Класс PXAuditLogger является singleton оболочкой класса {@link NLLogger}.
	 * Создание экзепляров класса должно производится через метод {@link PXAuditLogger::getLogger()}.
	 *
	 * @param PXApplication $app ссылка на PXApplication {@link $app}
	 * @param PXDatabase $db ссылка на PXDatabase {@link $db}
	 */
	public function __construct(public $app, public $db)
	{
		$this->logger = NLLogger::getLogger('audit');

		$loggerSqlFormatFields = [
			'ts' => '%t',
			'level' => '%l',
			'type' => '%1',
			'source' => '%2',
			'user' => '%3',
			'ip' => '%4',
			'description' => '%5',
			'diff' => '%6',
		];
		$loggerSqlFormat = $db->loggerSqlFormat('log_audit', $loggerSqlFormatFields);
		if (!$loggerSqlFormat || $this->app->getProperty('DISABLE_LOGGER', 0) == '1') {
			$this->logger->addAppender('sql', 'BlackHoleAppender');
		} else {
			$this->logger->addAppender('sql', 'SQLAppender');
			$this->logLevel = $this->logger->INFO;
			$this->logger->setLevel($this->logLevel);
			$this->logger->setAppenderParam('sql', 'format', $loggerSqlFormat);
			$this->logger->setAppenderParamByRef('sql', 'db', $this->db);
		}
	}

	/**
	 * Конструктор - синглетон.
	 *
	 * @param PXApplication $app ссылка на PXApplication {@link $app}
	 * @param PXDatabase $db ссылка на PXDatabase {@link $db}
	 * @return PXAuditLogger - single instance of PXauditLogger.
	 */

	public static function getLogger()
	{
		static $instance;

		if ($instance === null) {
			$app = PXRegistry::getApp();
			$db = PXRegistry::getDb();

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
	public function debug($description, $source = 'UNKNOWN', $type = 'AUDIT', $diff = NULL)
	{
		$this->logger->debug($this->_createMsg($description, $source, $type, $diff));
	}

	/**
	 * Метод оболочка для вызова {@link NLLogger::info()}.
	 * Параметры описаны в {@link PXAuditLogger::debug()}
	 */
	public function info($description, $source = 'UNKNOWN', $type = 'AUDIT', $diff = NULL)
	{
		$this->logger->info($this->_createMsg($description, $source, $type, $diff));
	}

	/**
	 * Метод оболочка для вызова {@link NLLogger::warn()}.
	 * Параметры описаны в {@link PXAuditLogger::debug()}
	 */
	public function warn($description, $source = 'UNKNOWN', $type = 'AUDIT', $diff = NULL)
	{
		$this->logger->warn($this->_createMsg($description, $source, $type, $diff));
	}

	/**
	 * Метод оболочка для вызова {@link NLLogger::error()}.
	 * Параметры описаны в {@link PXAuditLogger::debug()}
	 */
	public function error($description, $source = 'UNKNOWN', $type = 'AUDIT', $diff = NULL)
	{
		$this->logger->error($this->_createMsg($description, $source, $type, $diff));
	}

	/**
	 * Метод оболочка для вызова {@link NLLogger::fatal()}.
	 * Параметры описаны в {@link PXAuditLogger::debug()}
	 */
	public function fatal($description, $source = 'UNKNOWN', $type = 'AUDIT', $diff = NULL)
	{
		$this->logger->fatal($this->_createMsg($description, $source, $type, $diff));
	}

	/**
	 * Формирует сообщение для записи в лог.
	 *
	 * @param String $description - текстовое сообщение
	 * @param String $source - источник сообщения (обычно datatype/id)
	 * @param String $type - текстовый тип сообщения AUDIT, USER, итп.
	 * @return Array - массив из 5 элементов для передачи в NLLogger->debug|...|fatal()
	 */
	public function _createMsg($description, $source, $type, $diff)
	{
		$user = $this->db->user->isAuthed() ? $this->db->user->login : 'UNKNOWN';
		$ip = PXRequest::GetRemoteAddr() ?: '127.0.0.1'; // for console scripts logging

		return [$type, $source, $user, $ip, $description, $diff];
	}

	/**
	 * Static метод, включение логгера
	 */
	public function on()
	{
		if (PXAuditLogger::disabled()) {
			$self = PXAuditLogger::getLogger();
			$self->logger->setLevel($self->logLevel);
		}
	}

	/**
	 * Static метод, отключение логгера
	 */
	public function off()
	{
		$self = PXAuditLogger::getLogger();
		$self->logger->setLevel($self->logger->NONE);
	}

	/**
	 * Static метод, возвращает true если лог не ведётся
	 */
	public function disabled()
	{
		$self = PXAuditLogger::getLogger();
		return $self->logger->NONE == $self->logger->getLevel();
	}
}
