<?php

require_once __DIR__ . '/common.defines.inc';

require_once BASEPATH . 'vendor/autoload.php';
require_once PPLIBPATH . 'common.version.inc';

require_once PPLIBPATH . 'registry.class.inc';
require_once PPLIBPATH . 'Common/functions.inc';
require_once PPLIBPATH . 'Debug/functions.inc';

require_once PPLIBPATH . 'loader.class.inc';

require_once PPLIBPATH . 'Filesys/dir.class.inc';
require_once PPLIBPATH . 'Request/classes.inc';
require_once PPLIBPATH . 'Mail/message.class.inc';
require_once PPLIBPATH . 'Mail/EventCalendar/classes.inc';
require_once PPLIBPATH . 'Logger/nllogger.class.inc';

require_once PPLIBPATH . 'Config/application.class.inc';
require_once PPLIBPATH . 'Database/database.class.inc';
require_once PPLIBPATH . 'User/classes.inc';

require_once PPLIBPATH . 'DisplayType/classes.inc';
require_once PPLIBPATH . 'StorageType/classes.inc';

require_once PPLIBPATH . 'Triggers/classes.inc';

require_once PPLIBPATH . 'abstractcronrun.class.inc';

require_once PPLIBPATH . 'search.class.inc';
require_once PPLIBPATH . 'logger.class.inc';
require_once PPLIBPATH . 'Objects/classes.inc';

require_once PPLIBPATH . 'Engine/classes.inc';

require_once PPLIBPATH . 'XML/classes.inc';

require_once PPLIBPATH . 'LazyLoader/blockcontent.class.inc';

// TODO: should be ripped off after PSR-4 switch
spl_autoload_register([\PP\Plugin\AbstractPlugin::class, 'autoload']);

// TODO: syslog udp with custom source hostname
// syslog udp packet should be in full form:
// @see https://github.com/Dalee/node-logger/blob/master/test/unit/adapters/syslog.test.js#L44
$logger = new Monolog\Logger(LOGGER_APP);
$logger->pushHandler(new Monolog\Handler\StreamHandler(CACHE_PATH . '/application.log'));
Monolog\Registry::addLogger($logger);

$logger = new Monolog\Logger(LOGGER_CRON);
$logger->pushHandler(new Monolog\Handler\StreamHandler(CACHE_PATH . '/cron.log'));
Monolog\Registry::addLogger($logger);
