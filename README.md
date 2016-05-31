# Разработка

В корне проекта выполнить команду:
```bash
$ git clone git@git.dalee.ru:pp/core.git pp-core
$ rm -rf ./libpp && composer install
```

Это создаст локальную версию pp-core для разработки, изменения в pp-core будут
автоматически синхронизироваться с `libpp`.
Подробнее на английском: https://carlosbuenosvinos.com/working-at-the-same-time-in-a-project-and-its-dependencies-composer-and-path-type-repository/
