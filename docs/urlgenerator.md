# Генерация ссылок

## Генератор

Для генерации ссылок используется генератор `PP\Lib\UrlGenerator\UrlGenerator`

Для инициализации генератора необходим контекст `PP\Lib\UrlGenerator\ContextUrlGenerator`. 

Генератор имеет следующий интерфейс:

 * getUserGenerator() - получить генератор ссылок на основе заданного контекста для пользовательского пространства
 * getAdminGenerator() - получить генератор ссылок на основе заданного контекста для пространства администратора
 * getContext() - получить текущий контекст
 * setContext() - изменить контекст (следует знать, что при смене контекста происходит очистка хранимых инстансов пользовательского и админского генераторов)

Генераторы пользовательского пространства и пространства администратора реализуют интерфейс `PP\Lib\UrlGenerator\GeneratorInterface`
 * generate($params = [])
 * indexUrl($params = [])
 * actionUrl($params = [])
 * jsonUrl($params = [])
 * popupUrl($params = [])

`$params` - параметры, способные переопределить параметры контекста

## Контекст

`PP\Lib\UrlGenerator\ContextUrlGenerator`:

Контекст содержит 4 характеристики:
 * targetAction - действие, которое необходимо выполнить в модуле (index, action, json, popup)
 * request - объект класса `PXRequest`
 * targetModule - `area` целевого модуля
 * currentModule - `area` текущего модуля
 
При генерации ссылок, если целевой модуль задан, то ссылка генерируется на него, если не задан - то ссылка генерируется на текущий модуль.
Если ни целевой, ни текущий модели не заданы в контексте, то будет брошено исключение `LogicException`

## Пример использования

```
$context = new \PP\Lib\UrlGenerator\ContextUrlGenerator();
$context->setTargetModule('test_module_area');
		
$generator = new \PP\Lib\UrlGenerator\UrlGenerator($context);

$generator->getAdminGenerator()->indexUrl();
$generator->getUserGenerator()->actionUrl();
```
