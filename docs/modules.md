# Модули

## Описание

Модули ядра, так же как и пользовательские должны находиться в папке `src` для автоматической загрузки
(в `namespace` `Module`). Для того чтобы описать модуль, используется `yaml` файл, расположенный в локальном проекте в
папке `app/config/modules.yml`.

Пример описания модуля:

```yaml
modules:
  main:
    description: "Главная"
    class: PP\Module\MainModule
    params:
      rootFormat: struct

bindings:
  main:
    - { module: main }
```

Соответственно описанный модуль должен находится в папке `libpp/src/Module/MainModule.php` и наследоваться от
абстрактного:

```php
<?php

namespace PP\Module;

class MainModule extends AbstractModule {
	
	// ...
	
}
```

Класс модуля обязательно должен постфиксоваться словом `Module`.

**На данный момент, плагины загружают свои модули сами!**
