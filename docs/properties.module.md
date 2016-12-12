# Модуль properties

Модуль настроек сайта. В качестве бэкенда для хранения используется таблица `sys_properties`, SQL базы сайта.
Название таблицы определено как константа `DT_PROPERTIES`.

## Особенности работы

 * Для использования `$app->getProperty` необходимо чтобы класс был инициализирован одним из `PXEngine`
 * Для `Smarty` создана функция и модификатор `property`
 * Параметры в названии которых присутствует префикс `SYS_` и описанные в `modules.xml` трактуются как системные
 * Для редактирования системных параметров используется ACL правило модуля `properties`
 * StorageType всегда строка
 
## Описание параметров и режим работы

Пример описания:
```
<module name="properties" description="Параметры" class="PXModuleProperties">
	<attribute>name=CHILDREN_ON_PAGE|description=Количество элементов на странице (админка)|displaytype=TEXT</attribute>
	<attribute>name=LINKS_ON_PAGE|description=Количество элементов на странице (админка)|displaytype=TEXT</attribute>

	<attribute>name=SAMPLE_CHECKBOX|description=Тестовый параметр|displaytype=CHECKBOX</attribute>
	<attribute>name=SAMPLE_TEXTAREA1|description=Тестовый параметр|displaytype=TEXT</attribute>
	<attribute>name=SAMPLE_TEXTAREA2|description=Тестовый параметр|displaytype=TEXT,500,100</attribute>
	<attribute>name=SYS_FIELD|description=Тестовый системный параметр|displaytype=TEXT</attribute>
</module>
```

Редактирование параметра SYS_FIELD доступно только группе пользователей с разрешенным
правилом `"Редактирование системных параметров"`

Редактирование параметров SAMPLE_* доступно группе `Администраторы`

Все параметры находящиеся в таблице `DT_PROPERTIES` и не описанные в параметрах модуля, 
по умолчанию трактуются как системные.
