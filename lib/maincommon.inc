<?php

require_once dirname(__FILE__) . '/common.defines.inc';

require_once BASEPATH . 'vendor/autoload.php';
require_once PPLIBPATH . 'common.version.inc';

require_once PPLIBPATH . 'registry.class.inc';
require_once PPLIBPATH . 'Common/functions.inc';
require_once PPLIBPATH . 'Debug/functions.inc';

require_once PPLIBPATH . 'loader.class.inc';

require_once PPLIBPATH . 'Filesys/dir.class.inc';
require_once PPLIBPATH . 'Datastruct/tree.classes.inc';
require_once PPLIBPATH . 'response.class.inc';
require_once PPLIBPATH . 'Request/classes.inc';
require_once PPLIBPATH . 'Mail/message.class.inc';
require_once PPLIBPATH . 'Mail/EventCalendar/classes.inc';
require_once PPLIBPATH . 'Logger/nllogger.class.inc';
require_once PPLIBPATH . 'monolog.php';

require_once PPLIBPATH . 'Config/application.class.inc';
require_once PPLIBPATH . 'Database/database.class.inc';
require_once PPLIBPATH . 'User/classes.inc';

require_once PPLIBPATH . 'session.class.inc';

require_once PPLIBPATH . 'DisplayType/classes.inc';
require_once PPLIBPATH . 'StorageType/classes.inc';

require_once PPLIBPATH . 'Triggers/classes.inc';

require_once PPLIBPATH . 'abstractmodule.class.inc';
require_once PPLIBPATH . 'abstractcronrun.class.inc';
require_once PPLIBPATH . 'abstractplugin.class.inc';

require_once PPLIBPATH . 'search.class.inc';
require_once PPLIBPATH . 'logger.class.inc';
require_once PPLIBPATH . 'Objects/classes.inc';

require_once PPLIBPATH . 'Engine/classes.inc';

require_once PPLIBPATH . 'XML/classes.inc';

require_once PPLIBPATH . 'LazyLoader/blockcontent.class.inc';

require_once PPLIBPATH . 'YAML/classes.inc';

spl_autoload_register(array('PXAbstractPlugin', 'autoload'));
