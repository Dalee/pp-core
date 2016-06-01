<?php

/**
 * Определяем основные логгеры приложения
 */
use Monolog\Logger;
use Monolog\Handler\StreamHandler;
use Monolog\Registry;

$logger = new Logger(LOGGER_APP);
$logger->pushHandler(new StreamHandler(BASEPATH . '/site/var/application.log'));
Registry::addLogger($logger);

$logger = new Logger(LOGGER_CRON);
$logger->pushHandler(new StreamHandler(BASEPATH . '/site/var/cron.log'));
Registry::addLogger($logger);
