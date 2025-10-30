# CMake generated Testfile for 
# Source directory: /home/runner/work/ptusa_main/ptusa_main
# Build directory: /home/runner/work/ptusa_main/ptusa_main/_codeql_build_dir
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
include("/home/runner/work/ptusa_main/ptusa_main/_codeql_build_dir/main_test[1]_include.cmake")
add_test(run_no_command_line_arguments "ptusa_main")
set_tests_properties(run_no_command_line_arguments PROPERTIES  PASS_REGULAR_EXPRESSION "(.*)
  -s, --script arg          The script file to execute \\(default: main\\.plua\\)
  -d, --debug               Enable debugging
      --no_io_nodes         No communicate with I\\\\O nodes
      --read_only_io_nodes  Read only from I\\\\O nodes
  -p, --port arg            Param port \\(default: 10000\\)
  -h, --help                Print help info
  -r, --rcrc                Reset params
      --opc-r               Start OPC UA server with program start \\(only 
                            read\\)
      --opc-rw              Start OPC UA server with program start 
                            \\(read-write\\)
      --sys_path arg        Sys path \\(default: \\./sys\\)
      --path arg            Path \\(default: \\.\\)
      --extra_paths arg     Extra paths \\(default: \\./dairy-sys\\)
      --sleep_time_ms arg   Sleep time, ms \\(default: 2\\)
" _BACKTRACE_TRIPLES "/home/runner/work/ptusa_main/ptusa_main/CMakeLists.txt;431;add_test;/home/runner/work/ptusa_main/ptusa_main/CMakeLists.txt;0;")
add_test(T1-PLCnext-Demo_main "/home/runner/work/ptusa_main/ptusa_main/_codeql_build_dir/Release/ptusa_main" "main.plua1")
set_tests_properties(T1-PLCnext-Demo_main PROPERTIES  PASS_REGULAR_EXPRESSION "main.plua1: No such file or directory" WORKING_DIRECTORY "/home/runner/work/ptusa_main/ptusa_main/demo_projects/T1-PLCnext-Demo" _BACKTRACE_TRIPLES "/home/runner/work/ptusa_main/ptusa_main/CMakeLists.txt;499;add_test;/home/runner/work/ptusa_main/ptusa_main/CMakeLists.txt;0;")
add_test(Busted-test "/home/runner/work/ptusa_main/ptusa_main/_codeql_build_dir/Release/lua" "-e" "package.path = package.path..';/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/busted/?/init.lua'" "-e" "package.path = package.path..';/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/busted/?.lua'" "-e" "package.path = package.path..';/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/penlight/lua/?.lua'" "/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/busted/busted.lua")
set_tests_properties(Busted-test PROPERTIES  _BACKTRACE_TRIPLES "/home/runner/work/ptusa_main/ptusa_main/CMakeLists.txt;505;add_test;/home/runner/work/ptusa_main/ptusa_main/CMakeLists.txt;0;")
add_test(T1-PLCnext-Demo "/home/runner/work/ptusa_main/ptusa_main/_codeql_build_dir/Release/lua" "-e" "package.path = package.path..';./?.lua'" "-e" "package.path = package.path..';/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/busted/?/init.lua'" "-e" "package.path = package.path..';/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/busted/?.lua'" "-e" "package.path = package.path..';./?.lua;/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/penlight/lua/?.lua;/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/luasystem/luasystem/?/init.lua;/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/mediator/src/?.lua;/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/term/term/?/init.lua;/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/term/term/?.lua;/home/runner/work/ptusa_main/ptusa_main/_codeql_build_dir/Release/?.lua;/home/runner/work/ptusa_main/ptusa_main/_codeql_build_dir/Release/?/init.lua;/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/say/say/src/?/init.lua;/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/dkjson/?.lua;/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/cliargs/src/?.lua'" "-e" "package.cpath = package.cpath..';/home/runner/work/ptusa_main/ptusa_main/_codeql_build_dir/Release/?.so'" "/home/runner/work/ptusa_main/ptusa_main/demo_projects/T1-PLCnext-Demo/spec/ptusa_main.lua")
set_tests_properties(T1-PLCnext-Demo PROPERTIES  WORKING_DIRECTORY "/home/runner/work/ptusa_main/ptusa_main/demo_projects/T1-PLCnext-Demo" _BACKTRACE_TRIPLES "/home/runner/work/ptusa_main/ptusa_main/CMakeLists.txt;511;add_test;/home/runner/work/ptusa_main/ptusa_main/CMakeLists.txt;0;")
add_test(T1-PLCnext-Demo_valgrind "valgrind" "--error-exitcode=1" "--num-callers=100" "--leak-check=full" "--verbose" "--track-origins=yes" "--show-leak-kinds=all" "/home/runner/work/ptusa_main/ptusa_main/_codeql_build_dir/Release/lua" "-e" "package.path = package.path..';/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/busted/?/init.lua'" "-e" "package.path = package.path..';/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/busted/?.lua'" "-e" "package.path = package.path..';./?.lua;/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/penlight/lua/?.lua;/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/luasystem/luasystem/?/init.lua;/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/mediator/src/?.lua;/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/term/term/?/init.lua;/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/term/term/?.lua;/home/runner/work/ptusa_main/ptusa_main/_codeql_build_dir/Release/?.lua;/home/runner/work/ptusa_main/ptusa_main/_codeql_build_dir/Release/?/init.lua;/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/say/say/src/?/init.lua;/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/dkjson/?.lua;/home/runner/work/ptusa_main/ptusa_main/deps/lunarmodules/cliargs/src/?.lua'" "-e" "package.cpath = package.cpath..';/home/runner/work/ptusa_main/ptusa_main/_codeql_build_dir/Release/?.so'" "/home/runner/work/ptusa_main/ptusa_main/demo_projects/T1-PLCnext-Demo/spec/ptusa_main.lua")
set_tests_properties(T1-PLCnext-Demo_valgrind PROPERTIES  WORKING_DIRECTORY "/home/runner/work/ptusa_main/ptusa_main/demo_projects/T1-PLCnext-Demo" _BACKTRACE_TRIPLES "/home/runner/work/ptusa_main/ptusa_main/CMakeLists.txt;521;add_test;/home/runner/work/ptusa_main/ptusa_main/CMakeLists.txt;0;")
add_test(save-speed-benchmark "/home/runner/work/ptusa_main/ptusa_main/_codeql_build_dir/Release/main_perfomance_test" "--benchmark_out=/home/runner/work/ptusa_main/ptusa_main/_codeql_build_dir/benchmark_result.json" "--benchmark_out_format=json" "--benchmark_context=pwd=`/home/runner/work/ptusa_main/ptusa_main/demo_projects/T1-PLCnext-Demo`" "--benchmark_counters_tabular=true")
set_tests_properties(save-speed-benchmark PROPERTIES  RUN_SERIAL "1" WORKING_DIRECTORY "/home/runner/work/ptusa_main/ptusa_main/demo_projects/T1-PLCnext-Demo" _BACKTRACE_TRIPLES "/home/runner/work/ptusa_main/ptusa_main/CMakeLists.txt;538;add_test;/home/runner/work/ptusa_main/ptusa_main/CMakeLists.txt;0;")
subdirs("deps/fmt")
subdirs("deps/lua")
subdirs("deps/toluapp")
subdirs("deps/zlib")
subdirs("deps/googletest")
subdirs("deps/subhook")
subdirs("deps/benchmark")
subdirs("deps/open62541")
subdirs("deps/cxxopts")
subdirs("deps/lunarmodules/luafilesystem")
subdirs("deps/lunarmodules/luasystem")
subdirs("deps/lunarmodules/term")
subdirs("deps/lunarmodules/luassert")
