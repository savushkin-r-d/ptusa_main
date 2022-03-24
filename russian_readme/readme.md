# Общее описание #

Управляющая логика реализуется в виде скрипта Lua. В ходе выполнения программы управление передается в отдельные специальные функции Lua, которые отвечают за отдельные этапы.

1. Общая схема работы управляющей программы:

![Clone repository](../readme_images/main.png)

2. Управляющий цикл выглядит так:

![Clone repository](../readme_images/control_cycle.png)

3. При включении/выключении операции происходит следующее:

![Clone repository](../readme_images/tech_object__set_mode.png)

4. При выполнении команды происходит следующее:

![Clone repository](../readme_images/tech_object__exec_cmd.png)


## Как собрать проект ##

Вы можете клонировать репозиторий при помощи следующей команды:
`git clone --recurse-submodules https://github.com/savushkin-r-d/ptusa_main.git`

Или, если вы клонировали проект без инициализированых и обновленных подмодулей, попробуйте следующее:
`git submodules update --init --recursive`

Если после рекурсивного клонирования всё равно возникают ошибки, следует очистить кэш CMake: 
 Проект -> Удалить кэш и выполнить перенастройку

### win ###

Требования:
1. Qt Creator

В Qt вы можете открыть файл CMakeList.txt клонированого репозитория.

Или клонировать репозиторий используя Qt:
    Добавить -> Импортировать проект -> Клонирование Git
    указать ссылку на репозиторий и поставить галочку для рекурсивного клонирования.

## Как запустить проект ##

Для запуска демо проекта вы должны использовать эту команду:

### win ###

 После сборки и развертывания *`windows-default`* конфигурации cmake:

```cmd
    bin\install\windows-default\Release\ptusa_main.exe demo_projects/T1-PLCnext-Demo/main.plua path demo_projects/T1-PLCnext-Demo/ sys_path demo_projects/T1-PLCnext-Demo/sys/ debug
```
