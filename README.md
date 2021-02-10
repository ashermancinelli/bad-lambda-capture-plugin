# Lambda Capture Checker

When using portability libraries such as RAJA[1] and Kokkos[2], the capture clauses of lambda statements are extremely important.
This repo provides a standalone executable and a clang plugin to verify that no array- or pointer-like member variables are captured in a lambda capture clause defined in an instance method.

## Building

Standard CMake workflow, and you must be using Clang of course:

```console
$ git clone https://github.com/ashermancinelli/lambda-capture-checker.git
$ cd bad-lambda-capture-plugin
$ export PROJ_DIR=$PWD
$ mkdir build && cd build
$ cmake .. && make
```

The rest of the documentation will assume this repository has been cloned to `$PROJ_DIR`.

*NOTE:* You may find warnings such as `ld: warning: direct access in function 'DeduceTemplateArgumentsByTypeMatch...`.
This is due to rtti being disables on OSX. Clang plugins assume you'll have clang on your path or you'll invoke the plugin with clang.
These warnings may be ignored.

## Usage

If you are familiar with clang plugins, this section may not be of much use to you.

### For a Project

The standalone driver may be run on a compile commands database.
If you are using a CMake-based project, simply add the following line to your top-level CMakeLists.txt file:

```cmake
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

Or pass `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON` to CMake invokation.

This will generate a file `compile_commands.json` in your top-level CMake build directory. 

### From Command Line

To run an example using the plugin library:

```console
$ clang++ -Xclang -load -Xclang $PROJ_DIR/build/src/liblambda-capture-checker.dylib -Xclang -plugin -Xclang lambda-capture-checker $PROJ_DIR/test/capture.cpp

/Users/manc568/workspace/clang-plugin/test/capture.cpp:17:8: error: Found lambda capturing pointer-like member variable here.

      *i = 1;
       ^
/Users/manc568/workspace/clang-plugin/test/capture.cpp:4:3: note: Member variable declared here:
  int *i;
  ^
/Users/manc568/workspace/clang-plugin/test/capture.cpp:16:22: remark: Consider creating a local copy of the member variable in local scope
just outside the lambda capture.
    auto throwaway = [=] () {
                     ^
```

To run an example using the stadalone plugin driver:

```console
$ $PROJ_DIR/build/src/lambda-capture-checker-standalone -f $PROJ_DIR/test/capture.cpp

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

## References

1. [RAJA](https://github.com/LLNL/RAJA/blob/main/docs/sphinx/user_guide/index.rst)
1. [Kokkos](https://github.com/kokkos/kokkos)
