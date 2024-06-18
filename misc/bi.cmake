# How to use to build and install targets for Windows (VS2022) and PLCnext
# (AXC F 2152, AXC F 1152):
# cmake -P ./misc/bi.cmake

execute_process(COMMAND
    ${CMAKE_COMMAND} -E env CLICOLOR_FORCE=1
    ${CMAKE_COMMAND} -E cmake_echo_color --cyan --bold "Start configure")

# Configure.
execute_process(
    COMMAND ${CMAKE_COMMAND} --preset windows-VS2022-default
        -D CMAKE_SKIP_INSTALL_ALL_DEPENDENCY=true -D CMAKE_RULE_MESSAGES=OFF
        -D CMAKE_BUILD_PARALLEL_LEVEL=8
    COMMAND ${CMAKE_COMMAND} --preset build-windows-AXCF2152-2022-LTS-Release
        -D CMAKE_SKIP_INSTALL_ALL_DEPENDENCY=true -D CMAKE_RULE_MESSAGES=OFF
        -D CMAKE_BUILD_PARALLEL_LEVEL=8
    COMMAND ${CMAKE_COMMAND} --preset build-windows-AXCF1152-2022-LTS-Release
        -D CMAKE_SKIP_INSTALL_ALL_DEPENDENCY=true -D CMAKE_RULE_MESSAGES=OFF
        -D CMAKE_BUILD_PARALLEL_LEVEL=8
    RESULT_VARIABLE res
    COMMAND_ECHO STDOUT)

if(NOT ${res} EQUAL 0)
    message(FATAL_ERROR "Error during configure!")
endif()

execute_process(COMMAND
    ${CMAKE_COMMAND} -E cmake_echo_color --cyan --bold "Start build")

# Build.
execute_process(
    COMMAND ${CMAKE_COMMAND} --build --preset build-windows-VS2022-release
        --target libptusa_main ptusa_main PtusaPLCnextEngineer
    COMMAND ${CMAKE_COMMAND} --build --preset build-windows-AXCF2152-2022-LTS-Release
        --target libptusa_main ptusa_main PtusaPLCnextEngineer
    COMMAND ${CMAKE_COMMAND} --build --preset build-windows-AXCF1152-2022-LTS-Release
        --target libptusa_main ptusa_main PtusaPLCnextEngineer
    RESULT_VARIABLE res
    COMMAND_ECHO STDOUT)

if(NOT ${res} EQUAL 0)
    message(FATAL_ERROR "Error during build!")
endif()

execute_process(COMMAND
    ${CMAKE_COMMAND} -E cmake_echo_color --cyan --bold "Start install")

# Install.
execute_process(
    COMMAND ${CMAKE_COMMAND} --build --preset build-windows-VS2022-release
        --target install
    COMMAND ${CMAKE_COMMAND} --build --preset build-windows-AXCF2152-2022-LTS-Release
        --target install
    COMMAND ${CMAKE_COMMAND} --build --preset build-windows-AXCF1152-2022-LTS-Release
        --target install
    RESULT_VARIABLE res
    COMMAND_ECHO STDOUT)

if(NOT ${res} EQUAL 0)
    message(FATAL_ERROR "Error during install!")
endif()
