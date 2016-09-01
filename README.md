# Разработка

В основном проекте, на период разработки, для зависимости `pp-core` выставить версию `dev-master`.
В корне проекта выполнить команду:
```bash
$ git clone git@git.dalee.ru:pp/core.git pp-core
$ rm -rf ./libpp && composer install
```

Это создаст локальную версию pp-core для разработки, изменения в pp-core будут автоматически синхронизироваться с `libpp`.
[Подробнее на английском](https://carlosbuenosvinos.com/working-at-the-same-time-in-a-project-and-its-dependencies-composer-and-path-type-repository/)

# Тестирование

Запуск всех тестов:

```bash
./vendor/bin/phpunit
```

Запуск тестов и генерация покрытия:

```bash
./vendor/bin/phpunit --coverage-html=./coverage
```

Запуск unit-тестов:

```bash
./vendor/bin/phpunit tests/Unit
```

Запуск конкретного тест-файла:

```bash
./vendor/bin/phpunit tests/Unit/PP/Datastruct/TreeTest.php
```

Запуск конкретного теста:

```bash
./vendor/bin/phpunit --filter=TreeTest::testToTableWithoutOrphans
```
