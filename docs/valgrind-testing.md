# Valgrind Memory Leak Testing

This document describes the Valgrind memory leak testing setup for the
ptusa_main project.

## Overview

Valgrind is a memory debugging tool that can detect memory leaks, memory
management bugs, and other memory-related issues. The project includes
automated Valgrind testing to ensure memory safety during test execution.

## Running Valgrind Tests Locally

### Prerequisites

Install Valgrind on your system:

```bash
# On Ubuntu/Debian
sudo apt-get install valgrind

# On Fedora/RHEL
sudo dnf install valgrind
```

### Running Tests

To run all tests with Valgrind memory leak checking:

```bash
cd build
ctest -R valgrind -V
```

To run only the main_test executable with Valgrind:

```bash
cd build
ctest -R main_test_valgrind -V
```

### Manual Valgrind Execution

You can also run Valgrind manually for more control:

```bash
cd build
valgrind --leak-check=full \
         --show-leak-kinds=definite,possible \
         --suppressions=../valgrind.supp \
         --track-origins=yes \
         --log-file=valgrind-custom.log \
         ./Release/main_test
```

## CI/CD Integration

Valgrind tests are automatically run in the GitHub Actions CI pipeline on
Linux builds. The workflow:

1. Installs Valgrind
2. Builds the project
3. Runs regular tests
4. Runs Valgrind memory leak tests
5. Reports any memory issues found

If memory leaks are detected, the CI logs will contain detailed
information about the leak locations.

## Suppression File

The `valgrind.supp` file in the project root contains suppressions for
known false positives from third-party libraries (Lua, Open62541,
GoogleTest, etc.).

### Adding New Suppressions

If you encounter false positives from third-party libraries:

1. Run Valgrind with `--gen-suppressions=all`
2. Identify the false positive in the output
3. Copy the generated suppression block
4. Add it to `valgrind.supp`
5. Add a comment explaining why the suppression is needed

Example suppression format:

```
{
   Description_of_false_positive
   Memcheck:Leak
   match-leak-kinds: definite
   fun:malloc
   fun:problematic_function
   obj:*/library_name.*
}
```

## Test Configuration

The Valgrind test is configured in `CMakeLists.txt` with the following
settings:

- **Error exit code**: 1 (test fails if leaks are found)
- **Leak check**: Full
- **Leak kinds checked**: definite, possible
- **Timeout**: 600 seconds
- **Log file**: `build/valgrind-main_test.log`

## Interpreting Results

### No Leaks Found

```
==12345== LEAK SUMMARY:
==12345==    definitely lost: 0 bytes in 0 blocks
==12345==    indirectly lost: 0 bytes in 0 blocks
==12345==      possibly lost: 0 bytes in 0 blocks
```

This indicates no memory leaks were detected.

### Leaks Found

```
==12345== 100 bytes in 1 blocks are definitely lost in loss record 1 of 1
==12345==    at 0x4C2FB0F: malloc (in /usr/lib/valgrind/vgpreload_memcheck.so)
==12345==    by 0x12345678: problematic_function (file.cpp:42)
```

This shows a memory leak and its location. Investigate and fix the
indicated code.

## Best Practices

1. **Run locally before pushing**: Always run Valgrind tests locally
   when making changes that could affect memory management.

2. **Keep suppressions minimal**: Only suppress known false positives
   from third-party libraries, never suppress actual leaks in your code.

3. **Document suppressions**: Always add comments explaining why a
   suppression is needed.

4. **Fix real leaks**: Don't suppress real memory leaks; fix them
   instead.

5. **Regular testing**: Run Valgrind tests regularly, especially after
   major refactoring.

## Troubleshooting

### Tests Time Out

Valgrind significantly slows down execution (10-50x). If tests timeout:

- Increase the timeout in CMakeLists.txt
- Run a subset of tests
- Use `--trace-children=no` to avoid tracing child processes

### False Positives

If you see leaks from third-party libraries:

1. Verify it's not your code causing the issue
2. Add a suppression to `valgrind.supp`
3. Document the reason for the suppression

### Performance Issues

Valgrind is slow. For faster iteration:

- Run Valgrind only on changed code paths
- Use `--leak-check=summary` for quick checks
- Run full checks before committing

## References

- [Valgrind Documentation](https://valgrind.org/docs/manual/manual.html)
- [Valgrind Quick Start Guide](https://valgrind.org/docs/manual/quick-start.html)
- [Memory Leak Detection with Valgrind](https://valgrind.org/docs/manual/mc-manual.html)
