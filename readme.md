# ptusa

[![GitHub Actions CI](https://github.com/savushkin-r-d/ptusa_main/actions/workflows/cmake.yml/badge.svg)](https://github.com/savushkin-r-d/ptusa_main/actions/workflows/cmake.yml) [![codecov](https://codecov.io/gh/savushkin-r-d/ptusa_main/branch/master/graph/badge.svg?token=G1BK5054IJ)](https://codecov.io/gh/savushkin-r-d/ptusa_main)

Russian readme is [here](russian_readme/readme.md).

## Common description ##

Controller logic implemented by Lua-script. At runtime, control is passed through special Lua functions that are responsible for the various stages of the control program.

1. The general schema of controller program work:

![Clone repository](readme_images/main_en.png)

2. Controller program loop looks like:

![Clone repository](readme_images/control_cycle_en.png)

3. When operation turn on/off happens next:

![Clone repository](readme_images/tech_object__set_mode_en.png)

4. During the execution of command happens next:
![Clone repository](readme_images/tech_object__exec_cmd_en.png)

## How to build a project ##

### win ###

Requirements:
1. Microsoft Visual Studio Community 2019

You should open (in Visual Studio) root directory. This you get a CMake based project.

### linux ###

Eclipse IDE with CMake based project on Linux.

## How to run a project ##

To run demo project you should use this command line:

### win ###

 After building and deploying *`windows-default`* cmake-configuration:

```cmd
    bin\install\windows-default\Release\ptusa_main.exe demo_projects/T1-PLCnext-Demo/main.plua path demo_projects/T1-PLCnext-Demo/ sys_path demo_projects/T1-PLCnext-Demo/sys/ debug
```
