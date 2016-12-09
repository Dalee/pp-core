# Модуль properties

Модуль настроек сайта. В качестве бэкенда для хранения используется таблица `sys_properties`, SQL базы сайта.
Название таблицы определено как константа `DT_PROPERTIES`.

## Особенности работы

 * Для использования `$app->getProperty` необходимо чтобы класс был инициализирован одним из `PXEngine`
 * Для `Smarty` создана функция и модификатор `property`
 * Параметры в названии которых присутствует префикс `SYS_` и описанные в `modules.xml` трактуются как системные
 * Для редактирования системных параметров используется ACL правило модуля `properties`
 
## Описание параметров и режим работы

Пример описания:
```
<module name="properties" description="Параметры" class="PXModuleProperties">
	<attribute>name=SAMPLE_CHECKBOX|description=Тестовый параметр|displaytype=CHECKBOX</attribute>
	<attribute>name=SAMPLE_TEXTAREA1|description=Тестовый параметр|displaytype=TEXT</attribute>
	<attribute>name=SAMPLE_TEXTAREA2|description=Тестовый параметр|displaytype=TEXT,500,100</attribute>
	<attribute>name=SYS_FIELD|description=Тестовый системный параметр|displaytype=TEXT</attribute>
</module>
```
 
Редактирование параметра SYS_PASSWORD доступно только группе пользователей с разрешенным
правилом `"Редактирование системных параметров"`

Редактирование параметров SAMPLE_* доступно группе `Администраторы`

Все параметры находящиеся в таблице `DT_PROPERTIES` по-умолчанию трактуются как системные
