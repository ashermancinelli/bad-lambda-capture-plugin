# Lambda Capture Checker

When using portability libraries such as RAJA[1] and Kokkos[2], the capture clauses of lambda statements are extremely important.
This repo provides a standalone executable and a clang plugin to verify that no array- or pointer-like member variables are captured in a lambda capture clause defined in an instance method.

## Building

Standard CMake workflow, and you must be using Clang of course:

```console
$ git clone https://github.com/ashermancinelli/lambda-capture-checker.git
$ cd lambda-capture-checker
$ export PROJ_DIR=$PWD
$ mkdir build && cd build
$ cmake .. && make
```

This project also exports CMake targets, so if you add install prefix to the cmake search path, you can simply import the target from your project.

The rest of the documentation will assume this repository has been cloned to `$PROJ_DIR`.

*NOTE:* You may find warnings such as `ld: warning: direct access in function 'DeduceTemplateArgumentsByTypeMatch...`.
This is due to rtti being disables on OSX. Clang plugins assume you'll have clang on your path or you'll invoke the plugin with clang.
These warnings may be ignored.

## Usage

If you are familiar with clang plugins, this section may not be of much use to you.

### For a Project

The standalone driver may be run on a compile commands database.
See references [3] and [4] for more information.
If you are using a CMake-based project, simply add the following line to your top-level CMakeLists.txt file:

```cmake
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

Or pass `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON` to CMake invokation.

This will generate a file `compile_commands.json` in your top-level CMake build directory.

### Using the python script

This is the method we use.
The script `check-all.py` in the top level directory loads a `compile_commands.json` database and appends the needed flags to the clang invokation.

```console
$ ./check-all.py --help
usage: ./check-all.py [options]

--------------------------------------------------------------------------------

Long Description:

    This tool reads each entry from a `compile_commands.json` database, and runs
    each command after appending the following flags:
        -Xclang -load
        -Xclang PLUGIN_LIBRARY
        -Xclang -plugin
        -Xclang PLUGIN_NAME

    The tool will log each source file the tool runs on.

--------------------------------------------------------------------------------

Check all files in compilation database with clang tool.

optional arguments:
  -h, --help            show this help message and exit
  -p COMPILE_COMMANDS, --compile-commands COMPILE_COMMANDS
                        Path to compilation database
  -l PLUGIN_LIBRARY, --plugin-library PLUGIN_LIBRARY
                        Path to clang plugin library
  -n PLUGIN_NAME, --plugin-name PLUGIN_NAME
                        Name of clang plugin
```

For example, to run this on HiOp's[5] codebase, we build with clang and export compile commands:

```
$ cd $HIOP_DIR/build
$ CC=clang CXX=clang++ cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
$ make
```

And then run the python script on each source file:

```
$ # CD into directory containing this tool
$ ./check-all.py -p $HIOP_DIR/build/compile_commands.json -l ./build/src/libLambdaChecker.dylib -n LambdaChecker
Running tool on /Users/manc568/workspace/hiop/src/Interface/chiopInterface.cpp
Running tool on /Users/manc568/workspace/hiop/src/Optimization/hiopNlpFormulation.cpp
Running tool on /Users/manc568/workspace/hiop/src/Optimization/hiopFactAcceptor.cpp
Running tool on /Users/manc568/workspace/hiop/src/Optimization/hiopIterate.cpp
...
```

### From Command Line

To run an example using the plugin library:

```console
$ # Set PROJ_DIR to top-level directory of this repository
$ clang++ -Xclang -load -Xclang $PROJ_DIR/build/src/libLambdaChecker.dylib -Xclang -plugin -Xclang LambdaChecker ../test/capture.cpp

../test/capture.cpp:17:8: error: Found lambda capturing pointer-like member variable here.

      *i = 1;
       ^
../test/capture.cpp:4:3: note: Member variable declared here:
  int *i;
  ^
../test/capture.cpp:16:22: remark: Consider creating a local copy of the member variable in local scope
just outside the lambda capture.
    auto throwaway = [=] () {
                     ^
../test/capture.cpp:24:7: error: Found lambda capturing pointer-like member variable here.

      j[0] = 1;
      ^
../test/capture.cpp:7:3: note: Member variable declared here:
  int j[1];
  ^
../test/capture.cpp:23:22: remark: Consider creating a local copy of the member variable in local scope
just outside the lambda capture.
    auto throwaway = [=] () {
                     ^
2 errors generated.
```

To run an example using the standalone plugin driver:

```console
$ $PROJ_DIR/build/src/LambdaCheckerStandalone $PROJ_DIR/test/capture.cpp

/Users/manc568/workspace/clang-plugin/test/capture.cpp:17:8: error: Found lambda capturing pointer-like member variable here.

      *i = 1;
       ^
/Users/manc568/workspace/clang-plugin/test/capture.cpp:4:3: note: Member variable declared here:
  int *i;
  ^
/Users/manc568/workspace/clang-plugin/test/capture.cpp:16:22: remark: Consider creating a local copy of the member variable in local scope
just outside the lambda capture.
    auto throwaway = [=] () {
```

To use this directly, your source must be isolated (eg no includes or linking steps) or you must use json compile commands database.
It is reccommended to use the latter.

## Motivation

For example, the loop in the following method `times_constant` may cause confusing and indeterminate errors when used with RAJA, especially when the data does not reside on the host:

```cpp
struct Vector {
  Vector(std::size_t sz, int* data/*=pointer to data on device*/)
    : sz(sz), data(data) {}
  void times_constant(int factor) {
    RAJA::forall<RAJA::cuda_exec<128>>(RAJA::RangeSegment(0, sz),
      [=] (RAJA::Index_type i) {
        /* Here, `data` is captured implicitly via `this` pointer */
        data[i] *= factor;
      });
  }
private:
  std::size_t sz;
  int* data;
};
```

See `./test/capture.cpp` for problematic and non-problematic examples.

## References

1. [RAJA](https://github.com/LLNL/RAJA/blob/main/docs/sphinx/user_guide/index.rst)
1. [Kokkos](https://github.com/kokkos/kokkos)
1. [Clang compile commands database spec](https://clang.llvm.org/docs/JSONCompilationDatabase.html)
1. [Clang compile commands tutorial](https://clang.llvm.org/docs/HowToSetupToolingForLLVM.html)
1. [HiOp](https://github.com/LLNL/hiop)
1. [Clang AST Visitor documentation](https://clang.llvm.org/docs/RAVFrontendAction.html)
1. [Peter Goldsborough's C++Now talk on Clang/LLVM tools](https://www.youtube.com/watch?v=E6i8jmiy8MY&ab_channel=CppNow)
