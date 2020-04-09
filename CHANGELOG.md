# Change log

## [2.3.19] 2020-04-09
- Исправлено поведение сессионной авторизации в админке: инвалидация текущей сессии в случае смены IP и в случае отключения активного статуса юзера.

## [2.3.18] 2020-04-08
- Исправлена загрузка справочников для listed полей таблиц в админке

## [2.3.17] 2020-03-26
- Обновлен CK Editor до версии 4.14

## [2.3.16] 2019-10-17
- Обновлены зависимости компонентов symfony до 3.4.

## [2.3.15] 2019-10-17
- Добавлен logger сервис в контейнер.
- Все сервисы, тегированные 'logger.handler', добавляются хендлерами logger сервису.
- Добавлены глобальные константы в контейнер как параметры.

## [2.3.14] 2019-10-01
- В методе `PXDatabase::getAllLinks()` исправлена работа с кешом
- Изменён тип хранения IP адреса в БД с VARCHAR(32) на INET
- Исправлен фильтр по IP в модуле LogAudit, для корректного поиска по IP адресу с типом хранения в БД Postgresql INET

## [2.3.13] 2019-09-17
- Изменен механизм получения last insert id в драйвере postgresql.

## [2.3.12] 2019-03-12
- Исправлен memory leak в `PP\Lib\PersistentQueue\Job`, воркеры инстанциируются on demand вместо хранения в списке тасков

## [2.3.11] 2019-02-13
### PHP7 Compatibility fixes (1 changes)
- PXStorageTypeFilesarray - исправлена ошибка Warning: count(): Parameter must be an array or an object that implements Countable

## [2.3.10] 2019-02-05
- Усиление безопасности admin session (httponly, regenerate id после авторизации, secure если https и т.д.)
- Исправлено поведение заголовка X-Real-IP, когда он содержит список из X-Forwarded-For

## [2.3.9] 2019-01-29
- Фикс передачи аргумента `PXFileListing::setDecorator` по ссылке.

## [2.3.8] 2018-11-30
- Фикс использования non-static calls в DisplayType FilesArray.

## [2.3.7] 2018-06-05
- Фикс `PP\Lib\Cache\Driver\Predis::deleteGroup` чтобы вырезал префиксы перед удалением. 

## [2.3.6] 2018-06-04
- `PXTreeObjects::__isset` теперь проверяет ключ не только во внутреннем дереве, но и в самом себе.
Это исправляет косячное поведение при использовании `empty` с магическими геттерами.
- `PP\Lib\Cache\Driver\Predis` теперь использует `scan` вместо `keys` для удаления по паттерну.

## [2.3.5] 2018-05-28
- `CompileContainerCommand` все же должен наследоваться от `AbstractCommand` из-за деплоя на дев.

## [2.3.4] 2018-05-28
- `Engine` теперь регистрирует себя в `PXRegistry`, до начала инициализации приложения
- `PXApplication` более не хранит инстанс `Engine` в свойствах и следовательно, он не сериализуется
- Добавлена команда `CompileContainerCommand` для генерации контейнера
- Контейнер теперь достается из прегенерированного файла, если его нет, то компилируется по новой на каждый запрос
- Удалена legacy-папка `vendor/PEAR`
- Переименованы именные конструкторы в `__construct`

## [2.2.4] 2018-03-30
- Добавлена пачка классов для кастомизации сериализации при работе с драйверами кэшей
- В env-переменной `DATABASE_CACHE` при использовании dsn, можно указать опцию `serializator`

## [2.2.3] 2018-03-08
- Исправление: изменения файла lang.yaml (*.yaml) не сбрасывали Application Cache

## [2.2.2] 2018-02-20
- Добавлен новый redis-драйвер для кэширования (`predis`)

## [2.2.1] 2018-02-19
- Из `PXDatabase` убран метод `importBoolean()`, дублировавший функциональность метода `__call()`
- `PXDatabase::clearObjectTypeCache` теперь публичный метод для сброса кэша ручками после комплексного запроса

## [2.2.0] 2018-02-14
- Добавлена константа `RUNTIME_PATH`, которая указывает на `app/runtime`. В нее следуют класть файлы, которые должны
оставаться между релизами
- Логи крон-модуля (`cron.results` и `lock/cronrun`) теперь хранятся в `app/runtime`

## [2.1.4] 2018-01-22
- `PXApplication::getConfigurationPaths` теперь проверяет пути на существование

## [2.1.3] 2018-01-19
- Временный фикс восстановления engine'а из кэша

## [2.1.2] 2018-01-19
- Фикс включенного по-умолчанию file cache
- Удалена константа NOT_NULL
- Увеличен размер поля ip в таблице log_audit для хранения ipv6
- Кэширование в `PXApplication` убрано в отдельную фабрику `PP\ApplicationFactory`
- В системные триггеры добавлены обработчики событий `onBeforeModuleRun` и `onAfterModuleRun`
- В `PXRequest` добавлен метод `getRequestId()`, возвращающий уникальный идентификатор текущего запроса
- В `AbstractEngine` добавлен обработчик десериализации, компилирующий контейнер DI после восстановления engine'а из кэша

## [2.1.1] 2017-12-08
- Фикс переменных окружения при `db:migrate:up`.

## [2.1.0] 2017-12-08
- Инструменты для дебага, `symfony/var-dumper` и `PXErrorReporter` (dev-режим) удалены. Теперь
необходимо использовать плагин [pp/debug](http://git.dalee.ru/pp/debug).

## [2.0.1] 2017-12-08
- Добавлен вывод select для фильтра, если для атрибута установлено displaytype="DROPDOWN"
- Крон для запуска воркеров очереди теперь может принимать контейнер
если последний реализует `ContainerAwareInterface`.
- `PP\Lib\Engine\AbstractEngine` компилирует контейнер в конструкторе
последним шагом.
- Добавился метод `PXRequest::getHttpHeader`, который может получить заголовок по имени не
учитывая регистр.
- Phpdoc для `PP\Lib\Collection::filter` теперь возвращает `static`.

## [2.0.0] 2017-10-31
- Обновлена версия php до 5.6 - в composer.json прописана platform.php 5.6.31
- Обновлены мажорные версии зависимостей symfony:
    - `symfony/http-foundation` `~3.3.0` lock = `3.3.10`
        https://github.com/symfony/http-foundation/blob/master/CHANGELOG.md
    - `symfony/console` `~3.3.0` lock = `3.3.10`
        https://github.com/symfony/console/blob/master/CHANGELOG.md
    - `symfony/event-dispatcher` `~3.3.0` lock = `3.3.10`
        https://github.com/symfony/event-dispatcher/blob/master/CHANGELOG.md
    - `symfony/yaml` `~3.3.0` lock = `3.3.10`
        https://github.com/symfony/yaml/blob/master/CHANGELOG.md
    - `symfony/var-dumper` `~3.3.0` lock = `3.3.10`
        https://github.com/symfony/var-dumper/blob/master/CHANGELOG.md
    - `symfony/config` `~3.3.0` lock = `3.3.10`
        https://github.com/symfony/config/blob/master/CHANGELOG.md
    - `symfony/dependency-injection` `~3.3.0` lock = `3.3.10`
        https://github.com/symfony/dependency-injection/blob/master/CHANGELOG.md
- Добавлена dev зависимость `friendsofphp/php-cs-fixer` `^2.7` lock = `2.7.1`

### [BC] Встречено влияние следующих breaking changes:
- yaml parser кидает notice, если встречает не обернутое в кавычки значение, начинающееся с символа `%`.
- yaml parser кидает notice, если встречает дублирование в ключах.

## [1.10.5] 2017-11-19
- Добавлен вывод select для фильтра, если для атрибута установлено displaytype="DROPDOWN"

## [1.10.4] 2017-09-19
- Добавлена возможность указать таймаут подключения для кэш драйвера Redis.
Пример: `redis://127.0.0.1:6379/0?timeout=2.0`, значение по-умолчанию: `1.5`.

## [1.10.3] 2017-08-25
- Багфикс: REQUEST_URI должен содержать только корректные utf-8 последовательности.
В противном случае, отображать 404 страницу.

## [1.10.2] 2017-08-25
- Добавлен автоматический отчет после выполнения команд унаследованных от AbstractCommand
- Добавлены стандартные опции для команд:
    - `--mail` или `-m` для списка адресов. Опция приоритетнее соответствующих env переменной и property
    - `--send-report` или `-S` для разрешения отправки авто-отчета
- Добавлена environment переменная PP_COMMAND_REPORT_MAIL со списком e-mail адресов для отправки отчета.
Значение переменной приоритетнее аналогичного property.
- Добавлены properties для настройки отчета команд:
    - `SYS_COMMAND_REPORT_MAIL` : список адресов для отправки отчета
    - `SYS_COMMAND_REPORT_FROM` : обратный адрес
    - `SYS_PROJECT_NAME` : имя проекта

## [1.10.1] - 2017-07-21
- Redis cache driver fixes

## [1.10.0] - 2017-07-12
- Багфикс: команда db:migrate:up не передавала флаг не использования кэша (sic!)
- Константа `PPPATH` удалена
- Проверка `DB_*` убрана из `maincommon.inc`, функция `EnvLoader::inject` добавлена,
и должна быть использована каждый раз когда требуется доступ к environment переменным
- Замена `DB_*` на единую `DATABASE_DSN` формата: `pgsql://user:web@example.com:port/database?encoding=utf-8`
- Параметр `cache` для базы данных может быть указан как в `DATABASE_DSN` в виде
аргумента query string `&cache=file` либо отдельной переменной окружения: `DATABASE_CACHE`
- Переменная окружения имеет бОльший приоритет чем параметр `cache` в `DATABASE_DSN`
- Из `application.class.inc` убран код относящийся к плагину `blocks`
- В `PXApplication` добавлена функция для подгрузки дополнительных языковых массивов
(используется плагином `blocks` в `onAfterEngineStart` триггере)
- `AbstractBasicCommand` может быть использована в рамках Docker-билда без базы данных
и установленных переменных окружения
- `BASEPATH/tmp` - deprecated, `BASEPATH/app/cache/tmp.*` используется для хранения
временных данных
- Директория `CACHE_PATH` теперь установлена в `BASEPATH/app/cache/tmp.PHP_VERSION.CHARSET`
- Логгер `LOGGER_APP` и `LOGGER_CRON` по-умолчанию пишут в файлы
`CACHE_PATH/application.log` и `CACHE_PATH/cron.log` соответственно

## [1.9.4] - 2017-06-20
- MassChangeModule багфикс

## [1.9.3] - 2017-06-05
- Команда `pp` теперь подключает только `libpp/lib/mainadmin.inc`, `libpp/lib/mainuser.inc`
вместо `libpp/lib/maincommon.inc` и его локальной версии. Последние подключаются внутри, также как и
локальные `mainuser.inc` и `mainadmin.inc`. `vendor/autoload.php` тоже подключается внутри.

## [1.9.2] - 2017-06-02
- Добавлено поле `sys_meta` к таблице `queue_job`
- Поправлен phpdoc для `PP\Lib\Collection::map`
- Если передана переменная окружения `PP_DONT_FORCE_SUDO`, то выключается проверка текущего пользователя
- В список директорий для автоматического поиска `XML`-словарей добавлена `app/config`
- Экспериментальная поддержка PHP 7.1, переименованы классы:
	- `src/Lib/Auth/Null.php` => `src/Lib/Auth/NullAuth.php`
	- `src/Lib/Cache/Driver/Null.php` => `src/Lib/Cache/Driver/NullCache.php`
	- `src/Lib/Html/Layout/Null.php` => `src/Lib/Html/Layout/NullLayout.php`

## [1.9.1] - 2017-03-31
- Автоматически-загружаемые `XML`-словари теперь ищутся во всех конфигурационных папках:
    - `site/etc/`
    - `local/etc/`
    - `libpp/etc/`

## [1.9.0] - 2017-03-22
- Глобальная переделка загрузки модулей
  - [Документация](docs/modules.md)
- Удалена обработка `helpers` в настройках модуля `MassChangeModule`:
  - Не используется на проектах МегаФона
  - Написано очень давно и сейчас больше похоже на очень плохой код
  - Если требуется добавлять обработчики для мультиопераций, то просьба переписать (там не много)
  - Вшитые обработчики были вынесены в приватные функции
- Рефакторинг rss-модуля:
  - `PXModuleRSSEngine` => `PP\Module\RssEngineModule`
  - `PXRssXML` => `PP\Lib\Rss\AbstractRssNode`
  - `PXRssChannel` => `PP\Lib\Rss\RssChannel`
  - `PXRssItem` => `PP\Lib\Rss\RssItem`

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
