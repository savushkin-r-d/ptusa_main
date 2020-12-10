# Общее описание #

Управляющая логика реализуется в виде скрипта Lua. В ходе выполнения программы управление передается в отдельные специальные функции Lua, которые отвечают за отдельные этапы.

1. Общая схема работы управляющей программы:

![Clone repository](readme_images/main.png)

2. Управляющий цикл выглядит так:

![Clone repository](readme_images/control_cycle.png)

3. При включении/выключении операции происходит следующее:

![Clone repository](readme_images/tech_object__set_mode.png)

4. При выполнении команды происходит следующее:

![Clone repository](readme_images/tech_object__exec_cmd.png)

# How to build a project #

If you want use version of project for Windows, you should use Microsoft Visual Studio Community for build. For run this project on PLC use Eclipse IDE with CMake based on Linux.