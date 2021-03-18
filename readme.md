Russian readme is [here](russian_readme/readme.md).

# Common description #

Controller logic implemented by Lua-script. At runtime, control is passed through special Lua functions that are responsible for the various stages of the control program.

1. The general schema of controller program work:

![Clone repository](readme_images/main_en.png)

2. Controller program loop looks like:

![Clone repository](readme_images/control_cycle_en.png)

3. When operation turn on/off happens next:

![Clone repository](readme_images/tech_object__set_mode_en.png)

4. During the execution of command happens next:
![Clone repository](readme_images/tech_object__exec_cmd_en.png)

# How to build a project #

## win ##

Requirements:
1. Microsoft Visual Studio Community 2019

You should open (in Visual Studio) root directory. This you get a CMake based project.

## linux ##

Eclipse IDE with CMake based project on Linux.

# How to run a project #

To run demo project you should use this command line:

## win ##

```cmd
    "out\build\x64-Debug (по умолчанию)\PtusaPLCnextEngineer.exe" demo_project\main.plua path demo_project\ sys_path demo_project\sys\ debug
```
