# ptusa_main #

[![Open Hub Project Status](https://www.openhub.net/p/ptusa_main/widgets/project_thin_badge.gif)](https://www.openhub.net/p/ptusa_main/)
[![GitHub Actions CI](https://github.com/savushkin-r-d/ptusa_main/actions/workflows/cmake.yml/badge.svg)](https://github.com/savushkin-r-d/ptusa_main/actions/workflows/cmake.yml)
[![codecov](https://codecov.io/gh/savushkin-r-d/ptusa_main/branch/master/graph/badge.svg?token=G1BK5054IJ)](https://codecov.io/gh/savushkin-r-d/ptusa_main)

[![Downloads](https://img.shields.io/github/downloads/savushkin-r-d/ptusa_main/total.svg)](https://github.com/savushkin-r-d/ptusa_main/releases/)

Russian readme is [here](russian_readme/readme.md).

## Content ##

1. [General system architecture](#general-system-architecture)
2. [Common description](#common-description)
3. [How to build the project](#how-to-build-the-project )
4. [Using PLCnext Technology C++ Toolchain](#using-plcnext-technology-c-toolchain)
5. [How to run a project](#how-to-run-a-project)
6. [Libraries used](#libraries-used)

## General system architecture ##

General system architecture is shown below:

<p align="center"><img src="readme_images/system_architecture_en.png"></p>

General system architecture when the control program is deployed on the server:

<p align="center"><img src="readme_images/system_architecture_2_en.png"></p>

## Common description ##

Controller logic implemented by Lua-script. At runtime, control is passed through special Lua functions that are responsible for the various stages of the control program.

+ The general schema of controller program work (in [Drakon](https://drakonhub.com) diagram language,  made with [Mermaid](https://mermaid.js.org/)):

<p align="center"><img src="docs/drakon_diagram/images/control_program_en.svg"></p>

+ Execution of the technological process:

``` mermaid
flowchart TD
    A([2 Process execution]) --- B[/For each unit\]
    B --- C[[3 Execution of the unit <br/> procedure]]
    C --- D[\Next unit/]
    D --- E([End])

```

``` mermaid
flowchart TD
    A([3 Execution <br/> of the unit procedure]) --- B[/For each operation\]
    B --- C[[4 Execution <br/> of the operation]]
    C --- D[\Next operation/]
    D --- E([End])

```

<p align="center"><img src="docs/drakon_diagram/images/operation_en.svg"></p>

``` mermaid

flowchart TD

    A([5 Execution <br/> of the steps]) --- A1[[6 Execution of the <br/> main step]]
    A1 --- B[/For each extra step\]
    B --- C[[6 Execution of the step]]
    C --- D[\Next extra step/]
    D --- E([End])

```

``` mermaid

flowchart TD

    A([6 Execution <br/> of the step]) --- B[/For each step\]
    B --- C[[7 Execution of the action]]
    C --- D[\Next action/]
    D --- E([End])

```

``` mermaid

flowchart TD

    A([7 Execution <br/> of the action]) --- B(Action logic)
    B --- E([End])

```

+ When operation is turned on (off) happens next:

<p align="center"><img src="readme_images/tech_object__set_mode_en.png"></p>

+ During the execution of command happens next:

<p align="center"><img src="readme_images/tech_object__exec_cmd_en.png"></p>

## How to build the project ##

You could clone repository by next command:
`git clone --recurse-submodules https://github.com/savushkin-r-d/ptusa_main.git`

or, if you are clonned without initializing and updating submodules, try next:
`git submodules update --init --recursive`

### win ###

Requirements:

+ Microsoft Visual Studio Community 2019

You should open (in Visual Studio) root directory. This you get a CMake based project.

+ Qt Creator

You could open file CMakeList.txt cloned repository in Qt.

Or clone repository in Qt:
    New -> Import Project -> Git Clone
    specify the repository link and check the box Recursive

+ Microsoft Visual Studio Community 2022

Before starting the project, you need to make sure that you have installed C++ CMake tools for Windows.
C++ CMake tools for Windows are installed using:

`Visual Studio Installer -> Development of classic applications in C++ -> C++ CMake tools for Windows`

Detailed installation information can be obtained [here](https://docs.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=msvc-160). The CMake version required for the build is 3.13 and above.

You should open (in Visual Studio) root directory. This you get a CMake based project.
If an error occurs after startup, then you should clear the CMake cache `Project -> Delete cache and perform reconfiguration`.

### linux ###

Eclipse IDE with CMake based project on Linux.

## Using PLCnext Technology C++ Toolchain ##

Download the latest versions from the Phoenix Contact website https://www.phoenixcontact.com/products (for example from the **AXC F 2152** area) ```PLCnext Technology C++ toolchain for Windows```(current is 2023.0 LTS) and ```sdk for Windows 64 bit```(current is 2022.0.4 LTS) , in the section Downloads->Software $-$ http://www.phoenixcontact.com/qr/2404267/softw.

Navigate to the folder where downloads are located (typical path `%userprofile%/Downloads`), unzip the archive (`SDK_2021.9_Windows_AXC_F_2152.tar.xz.zip`).

### Installing (updating) the SDK ###

Navigate to the folder where downloads are previously unzipped, Call the CLI in the console using the following command:

```ps
plcncli.exe install sdk –d [installation path] –p [path to archive file]
```

>If you install several SDKs, Phoenix Contact recommends to use the "target name/firmware version" folder structure.

Also your installation path must match with path in CMake.

E.g.:

```ps
plcncli.exe install sdk -d C:\CLI\sdks\AXCF2152\2022_0\ -p pxc-glibc-x86_64-mingw32-axcf2152-image-mingw-cortexa9t2hf-neon-axcf2152-toolchain-2022.0.tar.xz
```

>The SDK is specified to the controller. The full list of controllers can be found on the PHOENIX CONTACT International site ([Home > Products > PLCs and I/O systems > PLCnext Control > Product list PLCnext Technology components](https://www.phoenixcontact.com/online/portal/pi?1dmy&urile=wcm%3apath%3a/pien/web/main/products/list_pages/PLCnext_technology_components_P-21-14-01/f77f0eb0-2a70-40c3-8679-7df2450e26db)).

## How to run a project ##

To run demo project you should use this command line:

### windows ###

+ After building and deploying *`windows-default`* cmake-configuration:

```cmd
    bin\install\windows-default\Release\ptusa_main.exe demo_projects/T1-PLCnext-Demo/main.plua path demo_projects/T1-PLCnext-Demo/ sys_path demo_projects/T1-PLCnext-Demo/sys/ debug
```

+ Launching a project in Microsoft Visual Studio Community 2022: `Select a launch item -> ptusa_main.exe`.

## Libraries used ##

### lua ###

Lua is a scripting programming language. A characteristic feature of Lua is implementation of a large number of program entities with a minimum of syntactic means.

Thus, all composite user data types (arrays, structures, sets, queues, lists) are implemented through the mechanism of tables, and the mechanisms of object-oriented programming, including multiple inheritance, are implemented using metatables, which are also responsible for overloading operations and a number of other features.
Lua is a procedural dynamically typed modular language with automatic memory management. Includes basic elements to support functional and object programming styles. Thus, Lua can be called a multi-paradigm language. Built-in parallel programming tools allow you to write multithreaded programs only by means of the language, without accessing the operating system API or external libraries.

Since the main purpose of Lua is embedding, it has effective means of interlanguage interaction, focused mainly on calling C libraries and working in a C environment. The language supports a small number of built-in data types: boolean values, numbers, strings, functions, streams. There are no typical combined data structures, such as arrays, sets, lists, and records; instead of all of them, one basic Lua structure is used a table.

A separate type of *userdata* is designed specifically for low-level programming and data exchange with external code in other languages. Functions in Lua are objects of the first class, can be assigned and passed in parameters. Closures are supported, it is possible to create higher-order functions. The object system is prototypical, there is no explicit support for inheritance, but it is easily implemented using metatables.

Detailed information can be obtained [here](https://www.lua.org).

### tolua++ ###

Tolua is a tool that greatly simplifies the integration of C/C++ code with Lua. Based on a cleaned header file, tolua automatically generates the binding code to access C/C++ features from Lua. Using Lua API and tag method facilities, tolua maps C/C++ constants, external variables, functions, classes, and methods to Lua.
To use tolua, create a package file, a C/C++ cleaned header file, listing the constants, variables, functions, classes, and methods to want to export to the Lua environment. Then tolua parses this file and creates a C/C++ file that automatically binds the C/C++ code to Lua. If link the created file with our application, the specified C/C++ code can be accessed from Lua.

Detailed information can be obtained [here](https://web.tecgraf.puc-rio.br/~celes/tolua/tolua-3.2.html).

### zlib ###

Zlib is a software library used for data compression and is an abstraction of the DEFLATE compression algorithm used in their gzip file compression program.

Detailed information can be obtained [here](https://www.zlib.net).

### subhook ###

SubHook is a simple hooking library for C and C++ that works on Windows, Linux and macOS. It supports x86 only (32-bit and 64-bit).

Detailed information can be obtained [here](https://github.com/Zeex/subhook).

### googletest ###

Google Test is a library for unit testing in C++. Google Test is built on the xUnit testing methodology, that is, when individual parts of the program (classes, functions, modules) are tested separately from each other, in isolation. The library itself is developed with the active use of testing, when adding any parts to the official version, in addition to the code of the changes themselves, it is necessary to write a set of tests to confirm their correctness.

Detailed information can be obtained [here](https://github.com/google/googletest).

### google benchmark ###

Google benchmark is a microbenchmark support library. The library can be used with C++03. However, it requires C++11 to build, including compiler and standard library support.

Detailed information can be obtained [here](https://github.com/google/benchmark).
