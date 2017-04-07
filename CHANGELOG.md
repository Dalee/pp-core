# Change log

## [1.8.9] - UNRELEASED
- Если передана переменная окружения `PP_DONT_FORCE_SUDO`, то выключается проверка текущего пользователя
- В список директорий для автоматического поиска `XML`-словарей добавлена `app/config`

## [1.8.8] - 2017-03-31
- Автоматически-загружаемые `XML`-словари теперь ищутся во всех конфигурационных папках:
    - `site/etc/`
    - `local/etc/`
    - `libpp/etc/`

## [1.8.7] - 2017-03-21
- Замена старых генераторов ссылок на `PP\Lib\UrlGenerator\UrlGenerator`
	
## [1.8.6] - 2017-03-21
- Исправлены пробелы на табы в шаблоне миграции
- Поправлены тесты на `PersistentQueue\Job`
- Добавлен `Symfony DI Container` в модули, кроны и команды
  - [Небольшое описание с ссылками на документацию](docs/di.md)
- Добавлен генератор URL к методам модуля `PP\Lib\UrlGenerator\UrlGenerator`
  - [Документация](docs/urlgenerator.md)
- Удалены следующие методы из класса `PP\Module\AbstractModule`:
	- `buildAdminUrl`
	- `buildAdminIndexUrl`
	- `buildAdminActionUrl`
	- `buildAdminPopupUrl`

## [1.8.5] - 2017-03-13
- Пользователям добавлено поле `E-mail`
- Крон выполнения фоновых задач отсылает уведомление на почту создателя объекта `queue_job`
- Добавлен `DisplayType` `JobResult` для форматированного вывода результата воркера
- Добавлен объект `JobResult`, который можно получить через `Job` для удобного управления результатом
- Убрана попытка получить `WorkerInterface::class` т.к. не 5.6 

## [1.8.4] - 2017-03-03
- Исправлен `PersistentQueue`, раньше он был завязан на МегаФон. Изменился интерфейс и добавились тесты
- Добавлен крон для запуска воркеров из `PersistentQueue`
- Добавлены параметры БД в `phpunit.xml` и `maincommon.inc` в тестовый `bootstrap.php`

## [1.8.3] - 2017-02-17
- Добавлен метод `Collection::last`
- Реализован интерфейс `JsonSerializable` у класса `Collection`

## [1.8.2] - 2017-01-30
### Правки после интеграции с Product Catalog 
- Добавлена возможность выводить уведомления (`flash messages`) через сессии
- Некорректная работа класса `NLDBDescription` - пустые значения использовались для сборки строки подключения

## [1.8.1] - 2017-01-25
### Правки после интеграции с Unite (багфикс v1)
- Багфикс в ObjectCache
- `PXContentObjectsInterface` перенесен в `PP\Lib\Objects\ContentObjectsInterface`
- Сортировка списка миграций перед выполнением
- Убраны ссылки на `PXAbstractPlugin`
- Исправлена ошибка обязательного наличия файла `.env`
- Добавлена команда `pp db:get-property`
- Добавлен новый класс `AbstractBasicCommand` для команд которые не требуют доступа к App/Db
- Переименованы команды pp/core:
	- `pp db:get-property` - `pp db:property:get`
	- `pp db:set-property` - `pp db:property:set`
	- `pp db:fill-meta` - `pp db:fill:meta`
	- `pp db:fill-uuid` - `pp db:fill:uuid`

## [1.8.0] - 2017-01-24 
### СУБД и DotEnv релиз
- `lib/Cache/*` перенесена в PSR-4 `src/lib/Cache`
	- `ObjectCache` - `PP\Lib\Cache\ObjectCache`
	- `PXCache*` - `PP\Lib\Cache\Driver\*`
	- `IPXCache` - `PP\Lib\Cache\CacheInterface`
- `PXAbstractCronRun` — deprecated, использовать `PP\Cron\AbstractCron`
- `PXCronRule` — deprecated, использовать `PP\Cron\CronRule`
- `PXAbstractPlugin` — deprecated, использовать `PP\Plugin\AbstractPlugin`
- `NLPGSQLDatabase` — deprecated, использовать `PP\Lib\Database\AbstractSqlDatabase`
- `charcheck` - убран из всех форм, javascript и php кода
- Работа с файлом `database.ini` удалена, настройки подключения к базе хранятся в environment
	- [Документация](docs/configuration.md)
- В файле `app/config/commands.yml` больше не нужно перечислять команды из namespace `PP\`. 
Встроенные команды `pp/core` автоматически регистрируются.
- `sbin` - директория удалена
- Переписаны миграции, используется `pp` интерфейс для работы с миграциями `pp:migrate:*`
- `etc/sql/psql-basesystem.sql` теперь содержит таблицу для миграций
- Удалено:
	- `htdocs/js/cookie.js`
	- `htdocs/js/forum.js`
	- `css/ie6.css`
	- `anticheating`
	- устаревшие драйвера баз данных: `pgsqlcluster`, `mysql`, `mssql`, `sqlite`
	- `vendor/CSSMin`
	- `vendor/phpdoc`
	- `vendor/rels`
	- `vendor/simpletest`

## [v1.7.2] - 2016-12-23
### Багфикс релиз
- Исправлены ошибки при отсутствующих параметрах для модуля `properties`
- Рефакторинг - инициализация сессии
- Для старых `assets` удален прямой доступ к файлу `properties.ini`

## [v1.7.1] - 2016-12-12
### Багфикс релиз
- Добавлена команда для установки параметров из консоли
- Несколько фиксов для поддержки работы плагина `blocks`

## [v1.7.0] - 2016-12-12
### Изменения
- Для каждой сущности, необходимо поле sys_uuid. Для генерации значения используется `Ramsey\Uuid\Uuid::uuid4()`
- Добавлена подсистема для запуска команд `./vendor/bin/pp`. Список доступных команд `app/config/commands.yml`
- Оптимизирована работа с `sys_meta`, для работы в админке физическое наличие файлов на файловой системе — не требуется
- Конструктор `blockingnumbers` больше не содержит параметров, капча всегда сохраняется в общем кэше
- Добавлена функция `property` и модификатор `property` для доступа к параметрам из `Smarty`
- `PXApplication::properties` теперь объект `ArrayCollection` и имеет область видимости `protected`
- `PXApplication::langTree` теперь объект `ArrayCollection`
- Исправлен `run` на правильный `execute` в командах
- Небольшой тюнинг интерфейса
- Работа с файлом `properties.ini` удалена, настройки сайта хранятся в базе данных - [properties](docs/properties.module.md)
- Рефакторинг Xml классов:
  - Убрана поддержка устаревшего расширения `domxml`
  - `PXmlAbstract` -> `PP\Lib\Xml\AbstractXml`
  - `PXmlAbstractNode` -> `PP\Lib\Xml\AbstractXmlNode`
  - `PXml` -> `PP\Lib\Xml\Xml`
  - `IPXml` -> `PP\Lib\Xml\XmlInterface`
  - `IPXmlNode` -> `PP\Lib\Xml\XmlNodeInterface`
  - `PXmlErrors` -> `PP\Lib\Xml\XmlErrors`
  - `PXmlSimplexml` -> `PP\Lib\Xml\SimpleXml`
  - `PXmlSimplexmlNode` -> `PP\Lib\Xml\SimpleXmlNode`


## [v1.6.9] - 2016-08-03
### Изменения
- Добавлены классы и миграция для работы с очередями `PP\Lib\PersistentQueue`

## [v1.6.2] - 2016-06-03
### Изменения
- Вырезан myconv и связанные с ним методы [link](http://git.dalee.ru/pp/core/merge_requests/11)
- Убран модуль и триггер форума [link](http://git.dalee.ru/pp/core/merge_requests/14)
- `PXEngine` -> `PP\Lib\Engine\AbstractEngine`
- `PXAdminEngine*` -> `PP\Lib\Engine\Admin\`
- Настоящие сессии в административном интерфейсе, класс сессии доступен через `$engine->getSession()`

## [v1.6.1] - 2016-06-02
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
- Убран класс DeprecatedException [link](http://git.dalee.ru/pp/core/merge_requests/3)
- Замена `PXAbstractModule` на `PXModule` [link](http://git.dalee.ru/pp/core/merge_requests/3)
- Замена `PXModuleDescription->getInstance()` на `PXModuleDescription->getModule` [link](http://git.dalee.ru/pp/core/merge_requests/3)
- Из `vendor` удалены FPDF, JSON, Recaptcha [link](http://git.dalee.ru/pp/core/merge_requests/3)
- Удален NLHTTPClient [link](http://git.dalee.ru/pp/core/merge_requests/3)
- Удалены устаревшие функции `_PrepareWMLText`, `SortRussianByTitle`, `wapUtf8Cyr`, `utfDecode`, `utfEncode`, `array_combine`, `__json_encode_koi_k2u`, `__json_encode_koi_u2k`, `json_encode_koi`, `json_decode_koi` [link](http://git.dalee.ru/pp/core/merge_requests/3)

## [v1.5] - 2008 - Темные века
### Изменения
- Умеренно-хаотичное развитие
