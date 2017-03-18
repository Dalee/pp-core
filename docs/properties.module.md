# Модуль properties

Модуль настроек сайта. В качестве бэкенда для хранения используется таблица `sys_properties`, SQL базы сайта.
Название таблицы определено как константа `DT_PROPERTIES`.

## Особенности работы

 * Для использования `$app->getProperty` необходимо чтобы класс был инициализирован одним из `PXEngine`
 * Для `Smarty` создана функция и модификатор `property`
 * Параметры в названии которых присутствует префикс `SYS_` и описанные в `modules.yml` трактуются как системные
 * Для редактирования системных параметров используется ACL правило модуля `properties`
 * StorageType всегда строка
 
## Описание параметров и режим работы

Пример описания:
```
properties:
    description: "Параметры"
    class: PP\Module\PropertiesModule
    params:
      attribute:
        - "name=CHILDREN_ON_PAGE|description=Количество элементов на странице (админка)|displaytype=TEXT"
        - "name=LINKS_ON_PAGE|description=Количество ссылок в попапе (админка)|displaytype=TEXT"
        - "name=SAMPLE_CHECKBOX|description=Тестовый параметр|displaytype=CHECKBOX"
        - "name=SAMPLE_TEXTAREA1|description=Тестовый параметр|displaytype=TEXT"
        - "name=SAMPLE_TEXTAREA2|description=Тестовый параметр|displaytype=TEXT,500,100"
        - "name=SYS_FIELD|description=Тестовый системный параметр|displaytype=TEXT"
```

Редактирование параметра SYS_FIELD доступно только группе пользователей с разрешенным
правилом `"Редактирование системных параметров"`

Редактирование параметров SAMPLE_* доступно группе `Администраторы`

Все параметры находящиеся в таблице `DT_PROPERTIES` и не описанные в параметрах модуля, 
по умолчанию трактуются как системные.
