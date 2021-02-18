# Clang Plugin Overview

This project contains two plugins:

* [LambdaChecker](./doc/LambdaChecker.md)
* [SyntaxChecker](./doc/SyntaxChecker.md)

The above links link to local documentation relevant to each particular plugin.
The rest of this documentation applies to all plugins in this project.

# Building

```console
$ mkdir build
$ cd build
$ CC=clang CXX=clang++ cmake .. -GNinja -DLLVM_ROOT=/path/to/llvm
$ ninja 
```

Any build system may be used, I prefer ninja/samurai.
