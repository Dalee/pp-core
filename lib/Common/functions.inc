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

require_once PPLIBPATH . 'Common/functions.pp.inc';
require_once PPLIBPATH . 'Common/functions.etc.inc';
require_once PPLIBPATH . 'Common/functions.file.inc';
require_once PPLIBPATH . 'Common/functions.string.inc';
require_once PPLIBPATH . 'Common/functions.array.inc';
require_once PPLIBPATH . 'Common/functions.execution.inc';
require_once PPLIBPATH . 'Common/functions.date.inc';
require_once PPLIBPATH . 'Common/functions.validate.inc';
require_once PPLIBPATH . 'Common/functions.imageswork.inc';
require_once PPLIBPATH . 'Common/functions.json.inc';
