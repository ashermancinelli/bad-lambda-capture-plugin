# SyntaxChecker

This tool enforces certain style guidelines by emmitting warnings on
non-conformant code. All examples from the console are assumed to be in the
build directory inside the source directory.

## Example

After building against a recent llvm build, you have the following options
available:

```console
$ ./src/syntax/SyntaxCheckerStandalone --help
USAGE: SyntaxCheckerStandalone [options] <source0> [... <sourceN>]

OPTIONS:

Generic Options:

  --help                      - Display available options (--help-hidden for more)
  --help-list                 - Display list of available options (--help-list-hidden for more)
  --version                   - Display the version of this program

SyntaxChecker options:

  --class-names               - Check for class names to be in pascal case.
  --extra-arg=<string>        - Additional argument to append to the compiler command line
  --extra-arg-before=<string> - Additional argument to prepend to the compiler command line
  --field-names               - Check for field names to be in snake case with a trailing underscore.
  -p=<string>                 - Build path
  --strip=<prefix>            - Strip prefix from class/struct decls.
```

All the options are enabled by default, but you may enable/disable them as you
like.

All options disabled:

```console
$ ./src/syntax/SyntaxCheckerStandalone ../test/hiop.cpp --field-names=0 --class-names=0
```                                                                                    

Checking field names only:
                                                                                       
```console                                                                             
$ ./src/syntax/SyntaxCheckerStandalone ../test/hiop.cpp --field-names=1 --class-names=0
../test/hiop.cpp:3:3: warning: Found non-snake case class/struct/union member variable name. Please consider renaming.
  int MyMemberVariable;
  ^   ~~~~~~~~~~~~~~~~
      my_member_variable_
1 warning generated.
```

Checking class names only:

```console
$ ./src/syntax/SyntaxCheckerStandalone ../test/hiop.cpp --field-names=0 --class-names=1
../test/hiop.cpp:2:1: warning: Found non-pascal case class/struct/union name. Please consider renaming.
class hiop_matrixDense {
^     ~~~~~~~~~~~~~~~~
      HiopMatrixDense
1 warning generated.
```

Checking class names and stripping prefix:

```console
$ ./src/syntax/SyntaxCheckerStandalone ../test/hiop.cpp --field-names=0 --class-names=1 --strip=hiop
../test/hiop.cpp:2:1: warning: Found non-pascal case class/struct/union name. Please consider renaming.
class hiop_matrixDense {
^     ~~~~~~~~~~~~~~~~
      MatrixDense
1 warning generated.
```

Checking everything:

```console
$ ./src/syntax/SyntaxCheckerStandalone ../test/hiop.cpp --strip=hiop
../test/hiop.cpp:2:1: warning: Found non-pascal case class/struct/union name. Please consider renaming.
class hiop_matrixDense {
^     ~~~~~~~~~~~~~~~~
      MatrixDense
../test/hiop.cpp:3:3: warning: Found non-snake case class/struct/union member variable name. Please consider renaming.
  int MyMemberVariable;
  ^   ~~~~~~~~~~~~~~~~
      my_member_variable_
2 warnings generated.
```
