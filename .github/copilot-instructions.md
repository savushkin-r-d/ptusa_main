# GitHub Copilot Instructions for ptusa_main

## CRITICAL FIRST STEP - READ THESE INSTRUCTIONS FIRST

**⚠️ ALWAYS follow these instructions before attempting any fallback search or exploration.**

### Essential First Step: Git Submodule Initialization
**CRITICAL:** The build will fail without this step. Always run immediately after cloning:
```bash
git submodule update --init --recursive
```

## Project Overview

**ptusa_main** is an industrial automation C++ project for process control systems (APCS). It uses CMake for building, Google Test for unit testing, and Google Benchmark for performance testing. The control logic is implemented via Lua scripting.

### Key Technologies
- **Language:** C++17 (CMake minimum version 3.13)
- **Build System:** CMake with presets
- **Testing:** Google Test framework
- **Benchmarking:** Google Benchmark
- **Scripting:** Lua 5.1 for control logic
- **Platforms:** Linux (GCC) and Windows (Visual Studio)

## Repository Structure

```
ptusa_main/
├── .github/                 # GitHub workflows and configurations
├── PAC/                     # Main PAC (Programmable Automation Controller) code
├── PAC_control_projects/    # Control project templates
├── common/                  # Shared utilities and libraries
├── demo_projects/           # Demo applications and examples
│   └── T1-PLCnext-Demo/     # Main demo project
├── deps/                    # Third-party dependencies (submodules)
├── docs/                    # Documentation and diagrams
├── test/                    # Unit tests and test infrastructure
│   ├── device/              # Device-specific tests
│   ├── dll/                 # DLL tests
│   └── performance/         # Performance benchmarks
├── CMakeLists.txt           # Main CMake configuration
├── CMakePresets.json        # CMake presets for different platforms
└── readme.md                # Project documentation
```

## Build Commands and Timing Expectations

### ⚠️ NEVER CANCEL - Build Timeout Settings
**Set these minimum timeouts (actual times are much faster):**
- **Build commands:** 10+ minutes (actual: ~3 minutes for full build)
- **Test commands:** 5+ minutes (actual: ~7 seconds)
- **Performance benchmarks:** 3+ minutes (actual: ~2 seconds)

### Standard Build Process (CLI)

1. **Initialize submodules** (CRITICAL - must be first):
```bash
git submodule update --init --recursive
```

2. **Create build directory and configure:**
```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
```

3. **Build all targets** (~3 minutes):
```bash
cmake --build . -j$(nproc)
```

### Available Build Targets
- `ptusa_main` - Main executable
- `libptusa_main` - Static library
- `main_test` - Unit test suite
- `main_performance_test` - Performance benchmarks

### Platform-Specific Notes

#### Linux Build
- Requires: GCC with C++17 support, CMake 3.13+
- Dependencies: lua5.1-dev (install with `apt install lua5.1-dev`)
- Optional: valgrind for memory testing

#### Windows Build
- Visual Studio 2019+ or Qt Creator
- CMake tools for Windows required
- Open root directory as CMake project in Visual Studio

## Testing

### Unit Tests (~7 seconds)
```bash
cd build
./main_test
```

**Expected Results:**
- Passing all tests
- Failed tests: 3 (expected network-related failures)
- Duration: ~7 seconds

### Performance Benchmarks (~2 seconds)
```bash
cd build
./main_performance_test
```

## Demo Project Execution

### Running Demo Application
```bash
cd build
./ptusa_main \
    --path ../demo_projects/T1-PLCnext-Demo/ \
    --sys_path ../demo_projects/T1-PLCnext-Demo/sys/ \
    --debug \
    ../demo_projects/T1-PLCnext-Demo/main.plua
```

**Expected Behavior:**
- Application starts and loads Lua control scripts
- May show network timeout warnings (expected behavior)
- Demonstrates process control logic execution

### Help Command
```bash
cd build
./ptusa_main --help
```

## Development Workflow

### 1. Clean Build Validation
```bash
# Clean state
rm -rf build/
git submodule update --init --recursive

# Build from scratch
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)  # ~3 minutes

# Verify build
ls -la ptusa_main main_test main_performance_test
```

### 2. Test Suite Validation
```bash
cd build
./main_test  # Should complete in ~7 seconds
```

### 3. Performance Regression Check
```bash
cd build
./main_performance_test  # Should complete in ~2 seconds
```

### 4. Demo Application Test
```bash
cd build
./ptusa_main --path ../demo_projects/T1-PLCnext-Demo/ \
    --sys_path ../demo_projects/T1-PLCnext-Demo/sys/ \
    --debug ../demo_projects/T1-PLCnext-Demo/main.plua
# Ctrl+C to exit after seeing startup messages
```

## Dependency Management

### Required System Dependencies (Linux)
```bash
# Install required packages
sudo apt update
sudo apt install -y lua5.1-dev cmake build-essential
```

### Optional Development Tools
```bash
# For memory analysis
sudo apt install -y valgrind

# For debugging
sudo apt install -y gdb
```

### Git Submodules (CRITICAL)
All third-party dependencies are managed via git submodules in `/deps/`. 
**Always run after fresh clone:**
```bash
git submodule update --init --recursive
```

## Common Development Tasks

### Adding New Tests
1. Create test file in `/test/` directory
2. Follow naming convention: `*_tests.cpp` and `*_tests.h`
3. Use Google Test framework patterns
4. Add to CMakeLists.txt if needed

### Modifying Control Logic
1. Lua scripts are in demo projects
2. Main control logic in `/PAC/` directory
3. Build and test with demo project after changes

### Performance Analysis
1. Use built-in benchmarks: `./main_performance_test`
2. Profile with standard tools (gprof, perf, etc.)
3. Memory analysis with valgrind

## Troubleshooting

### Build Failures
1. **"No such file or directory" in deps/**: Run `git submodule update --init --recursive`
2. **CMake configuration fails**: Check CMake version >= 3.13
3. **Compilation errors**: Verify C++17 compiler support
4. **Linking errors**: Clean build directory and rebuild

### Test Failures
1. **Network-related test failures**: Expected (3/462 tests)
2. **Timeout errors**: Check system resources
3. **Lua script errors**: Verify lua5.1-dev installation

### Runtime Issues
1. **"Cannot find lua library"**: Install lua5.1-dev
2. **Demo project fails to load**: Check file paths and permissions
3. **Network timeouts in demo**: Expected behavior for offline demo

## CI/CD Integration

### GitHub Actions
The project uses GitHub Actions for continuous integration:
- Build validation on Linux and Windows
- Unit test execution
- Code coverage reporting (codecov)

### Expected CI Behavior
- Build time: ~3 minutes
- Test execution: ~7 seconds
- Performance tests: ~2 seconds
- Coverage reporting: Automatic

## Code Style and Standards

- If possible, limit sentence length to 80 characters per line

### C++ Standards
- C++17 features encouraged
- Follow existing naming conventions
- Use RAII and modern C++ practices
- Prefer standard library over custom implementations

### CMake Standards
- Minimum version 3.13
- Use modern CMake practices
- Maintain cross-platform compatibility
- Document custom functions and macros

### Lua Scripting
- Lua 5.1 compatibility required
- Follow project-specific control logic patterns
- Document complex automation sequences

### Documentation and Comments
- Limit sentence length to 80 symbols per line
- End comment line with a period

## Advanced Usage

### Cross-Compilation (PLCnext)
The project supports Phoenix Contact PLCnext controllers:
1. Install PLCnext C++ toolchain
2. Use appropriate SDK for target controller
3. Configure CMake with toolchain file

### Memory Analysis
```bash
cd build
valgrind --tool=memcheck --leak-check=full ./main_test
```

### Debug Builds
```bash
mkdir build-debug && cd build-debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . -j$(nproc)
gdb ./ptusa_main
```

---

## Quick Reference

### Essential Commands Sequence
```bash
# 1. CRITICAL FIRST STEP
git submodule update --init --recursive

# 2. Build (allow 10+ minutes timeout, actual ~3 minutes)
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(nproc)

# 3. Test (allow 5+ minutes timeout, actual ~7 seconds)
./main_test

# 4. Performance (allow 3+ minutes timeout, actual ~2 seconds)
./main_performance_test

# 5. Demo
./ptusa_main --path ../demo_projects/T1-PLCnext-Demo/ \
    --sys_path ../demo_projects/T1-PLCnext-Demo/sys/ \
    --debug ../demo_projects/T1-PLCnext-Demo/main.plua
```

### Key Statistics to Expect
- **Build time:** ~3 minutes (full clean build)
- **Unit tests:** all pass (~7 seconds)
- **Performance tests:** ~2 seconds
- **Total files:** ~200+ source files
- **Lines of code:** ~50,000+ lines

**Remember:** Always allow generous timeout margins in CI/CD and automation tools, even though actual execution times are much faster.
