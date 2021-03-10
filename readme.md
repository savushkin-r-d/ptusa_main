Russian readme is [here](russian_readme/readme.md).

# Common description #

Controller logic implemented by Lua-script. At runtime, control is passed through special Lua functions that are responsible for the various stages of the control program.

Path to solution (_.sln_) from repository root: PAC_control_projects/main_control_prg/main_control_prg_vc/main_control_prg_vc.sln.

1. The general schema of controller program work:

![Clone repository](readme_images/main_en.png)

2. Controller program loop looks like:

![Clone repository](readme_images/control_cycle_en.png)

3. When operation turn on/off happens next:

![Clone repository](readme_images/tech_object__set_mode_en.png)

4. During the execution of command happens next:
![Clone repository](readme_images/tech_object__exec_cmd_en.png)

# How to build a project #

If you want to use the version of project for Windows, you should use Microsoft Visual Studio Community for build. To run this project on PLC, use Eclipse IDE with CMake based project on Linux.
