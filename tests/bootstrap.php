<?php
/**
 * Do something useful here if required
 *
 */

// TODO: this is override common.defines thing, need to be refactored
define('PP_DONT_FORCE_SUDO', 1);

$_SERVER["REQUEST_METHOD"] = 'GET';
$_SERVER["DOCUMENT_ROOT"] = __DIR__ . '/../';

require __DIR__ . '/../lib/common.defines.inc';
require __DIR__ . '/../lib/maincommon.inc';
require __DIR__ . '/../vendor/autoload.php';
