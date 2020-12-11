Russian readme [here](russian_readme/readme.md).

# Common description #

Controller logic implemented by Lua-script. We transfer control during executing control program through special Lua functions, which responsible for different steps of the control program working.

1. Common schema of controller program logic works:

![Clone repository](readme_images/main_en.png)

2. Controller program loop look like:

![Clone repository](readme_images/control_cycle_en.png)

3. When operation turn on/off happens next:

![Clone repository](readme_images/tech_object__set_mode_en.png)

4. During the execution of command happens next:
![Clone repository](readme_images/tech_object__exec_cmd_en.png)

# How to build a project #

If you want to use the version of project for Windows, you should use Microsoft Visual Studio Community for build. To run this project on PLC, use Eclipse IDE with CMake based project on Linux.
