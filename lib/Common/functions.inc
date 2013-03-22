<?php
/**
 * Project:     Dalee PHP-classes library
 * File:        common.functions.inc
 *
 * @copyright   Dalee, 2003-2010
 * @author      Michael Beloshapka <mb@dalee.ru>
 * @package     ProximaPortal
 * @subpackage  Common
 */

if (!defined('IS_WIN')) {
	define('IS_WIN', (bool)(substr(PHP_OS, 0, 3) == 'WIN'));
}

require_once('functions.pp.inc');
require_once('functions.etc.inc');
require_once('functions.file.inc');
require_once('functions.string.inc');
require_once('functions.array.inc');
require_once('functions.execution.inc');
require_once('functions.date.inc');
require_once('functions.validate.inc');
require_once('functions.imageswork.inc');
require_once('functions.json.inc');
