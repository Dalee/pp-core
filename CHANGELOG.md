# Change log

## [Unreleased]
### Изменения
<<<<<<< fe0a0c663392026ff8c87b515128c561a5566740
- Вырезан myconv и связанные с ним методы [link](http://git.dalee.ru/pp/core/merge_requests/11)

## [v1.6.1]
=======
- `PXEngine` -> `PP\Lib\Engine\AbstractEngine`
- `PXAdminEngine*` -> `PP\Lib\Engine\Admin\`
- Настоящие сессии в административном интерфейсе, класс сессии доступен через `$engine->getSession()`

## [Unreleased]
>>>>>>> Session support for AdminEngine
### Изменения
- Monolog, `PXRegistry::getLogger($logger_name)`, где `$logger_name` может быть одной из констант: `LOGGER_APP`, `LOGGER_CRON`. По-умолчанию `LOGGER_APP` рендерится в `site/var/application.log`, `LOGGER_CRON` - `site/var/cron.log`
- Убрана поддержка PHP версии ниже 5.4
- Убрана поддержка Windows платформ
- Убраны старые варианты авторизации Rambler, Domain, Plain
- PXAuthNull, PXAuthSecure, PXAuthSession перенесены в PSR-4 (src/Lib/Auth)
- auth.ini в качестве названия механизма авторизации теперь требует полный PSR-4 путь к классу (для обратной совместимости - используется маппинг для secure и session вариантов авторизации)
- migrate.php больше не пытается обращаться к datatypes.xml
- Капча при доступе в административный интерфейс (в development-режиме не требует ввода)
- Определение development режима работы app->isDevelopmentMode(), установка режима через properties.ini `ENVIRONMENT=DEVELOPER`
- `class NLAbstractLayout` -> `abstract PP\Lib\Html\Layout\LayoutAbstract`
- `class PXAdminHTMLLayout` -> `class PP\Lib\Html\Layout\AdminHtmlLayout`
- `class PXLayoutInterface` -> `interface PP\Lib\Html\Layout\LayoutInterface`
- Версия pp/core отображается только для авторизированных пользователей
- Добавлена константа `APPPATH` и метод `getAppPath` в класс `PXApplication` для ее получения [link](http://git.dalee.ru/pp/core/merge_requests/10)

### Merge Request
- [!8](http://git.dalee.ru/pp/core/merge_requests/8)
- [!9](http://git.dalee.ru/pp/core/merge_requests/9)


## [v1.6] - 2016-05-17
### Изменения
- Убрана поддержка `handler` из `datatype.xml` [link](http://git.dalee.ru/pp/core/merge_requests/2)
- Убраны беполезные `handler` [link](http://git.dalee.ru/pp/core/merge_requests/1)
- Убарн класс DeprecatedException [link](http://git.dalee.ru/pp/core/merge_requests/3)
- Замена `PXAbstractModule` на `PXModule` [link](http://git.dalee.ru/pp/core/merge_requests/3)
- Замена `PXModuleDescription->getInstance()` на `PXModuleDescription->getModule` [link](http://git.dalee.ru/pp/core/merge_requests/3)
- Из `vendor` удалены FPDF, JSON, Recaptcha [link](http://git.dalee.ru/pp/core/merge_requests/3)
- Удален NLHTTPClient [link](http://git.dalee.ru/pp/core/merge_requests/3)
- Удалены устаревшие функции `_PrepareWMLText`, `SortRussianByTitle`, `wapUtf8Cyr`, `utfDecode`, `utfEncode`, `array_combine`, `__json_encode_koi_k2u`, `__json_encode_koi_u2k`, `json_encode_koi`, `json_decode_koi` [link](http://git.dalee.ru/pp/core/merge_requests/3)

## [v1.5] - 2008 - Темные века
### Изменения
- Умеренно-хаотичное развитие
