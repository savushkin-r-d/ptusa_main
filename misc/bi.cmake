# How to use to build and install targets for Windows (VS2022):
# cmake -D CFG_NAME:string="windows-VS2022-default" -D BUILD_NAME:string="build-windows-VS2022-release" -P ./misc/bi.cmake
# and PLCnext  (AXC F 2152, AXC F 1152):
# cmake -D CFG_NAME:string="build-windows-AXCF2152-2022-LTS-Release" -D BUILD_NAME:string="build-windows-AXCF2152-2022-LTS-Release" -P ./misc/bi.cmake
# cmake -D CFG_NAME:string="build-windows-AXCF1152-2022-LTS-Release" -D BUILD_NAME:string="build-windows-AXCF1152-2022-LTS-Release" -P ./misc/bi.cmake

# Save start time.
string(TIMESTAMP t "%s")

# Check for required parameters.
if (NOT DEFINED CFG_NAME AND NOT DEFINED BUILD_NAME)
    message(FATAL_ERROR "No presets are specified.")
endif()
message("Presets - ${CFG_NAME}, ${BUILD_NAME}")


# Configure.
string(TIMESTAMP t0 "%s")
message( "") # Just new line.
execute_process(COMMAND
    ${CMAKE_COMMAND} -E env CLICOLOR_FORCE=1
    ${CMAKE_COMMAND} -E cmake_echo_color --cyan --bold "\nStart configure...")

execute_process(
    COMMAND ${CMAKE_COMMAND} --preset ${CFG_NAME}
        -D CMAKE_SKIP_INSTALL_ALL_DEPENDENCY=true -DCMAKE_RULE_MESSAGES=OFF
    RESULT_VARIABLE res
    COMMAND_ECHO STDOUT
    COMMAND_ERROR_IS_FATAL ANY
    ENCODING UTF8)

string(TIMESTAMP t1 "%s")
math(EXPR elapsed "${t1} - ${t0}")
execute_process(COMMAND
    ${CMAKE_COMMAND} -E env CLICOLOR_FORCE=1
    ${CMAKE_COMMAND} -E cmake_echo_color --cyan --bold "Configure time ${elapsed} s.")


# Build.
string(TIMESTAMP t0 "%s")
message( "") # Just new line.
execute_process(COMMAND
    ${CMAKE_COMMAND} -E env CLICOLOR_FORCE=1
    ${CMAKE_COMMAND} -E cmake_echo_color --cyan --bold "Start build...")

# BUild target PtusaPLCnextEngineer only for PLCnext.
if(BUILD_NAME MATCHES ".*AXCF.*")
    set(PtusaPLCnextEngineer "PtusaPLCnextEngineer")
elseif()
    set(PtusaPLCnextEngineer "")
endif()

execute_process(
    COMMAND ${CMAKE_COMMAND} --build --preset ${BUILD_NAME}
        --target libptusa_main ptusa_main ${PtusaPLCnextEngineer} --parallel 8
    RESULT_VARIABLE res
    COMMAND_ECHO STDOUT
    COMMAND_ERROR_IS_FATAL ANY
    ENCODING UTF8)

string(TIMESTAMP t1 "%s")
math(EXPR elapsed "${t1} - ${t0}")
execute_process(COMMAND
    ${CMAKE_COMMAND} -E env CLICOLOR_FORCE=1
    ${CMAKE_COMMAND} -E cmake_echo_color --cyan --bold "Build time ${elapsed} s.")


# Install.
string(TIMESTAMP t0 "%s")
message("") # Just new line.
execute_process(COMMAND
    ${CMAKE_COMMAND} -E env CLICOLOR_FORCE=1
    ${CMAKE_COMMAND} -E cmake_echo_color --cyan --bold "Start install...")

execute_process(
    COMMAND ${CMAKE_COMMAND} --build --preset ${BUILD_NAME}
        --target install
    RESULT_VARIABLE res
    COMMAND_ECHO STDOUT
    COMMAND_ERROR_IS_FATAL ANY
    ENCODING UTF8)

string(TIMESTAMP t1 "%s")
math(EXPR elapsed "${t1} - ${t0}")
execute_process(COMMAND
    ${CMAKE_COMMAND} -E env CLICOLOR_FORCE=1
    ${CMAKE_COMMAND} -E cmake_echo_color --cyan --bold "Install time ${elapsed} s.")

math(EXPR elapsed_total "${t1} - ${t}")
execute_process(COMMAND
    ${CMAKE_COMMAND} -E env CLICOLOR_FORCE=1
    ${CMAKE_COMMAND} -E cmake_echo_color --cyan --bold "Total time ${elapsed_total} s.")
