# qore-llvm

[![Build Status](https://hq.qoretechnologies.com/jenkins/buildStatus/icon?job=qore-llvm)](https://hq.qoretechnologies.com/jenkins/job/qore-llvm/)

## Building

### CMake options

Option name       | Default | Comment
------------------|:-------:|--------------------------------------------------------------------------
`QORE_LOGGING`    |  `ON`   | Enable log messages
`QORE_COVERAGE`   |  `ON`   | Build with `--coverage` option. Not suitable for release.
`QORE_USE_LLVM`   |  `ON`   | Used for disabling the dependency on LLVM.
`QORE_ENABLE_JIT` |  `ON`   | Ignored if `QORE_USE_LLVM` is `OFF`. Disabling JIT makes builds faster.

### Out-of-source build using ninja

```bash
mkdir build
cd build
cmake -G "Ninja" ..
ninja
```

### Out-of-source build using make

```bash
mkdir build
cd build
cmake -G "Unix Makefiles" ..
make
```

### Eclipse (in-source)

```bash
cmake -G "Eclipse CDT4 - Unix Makefiles" .
```
or
```bash
cmake -G "Eclipse CDT4 - Ninja" .
```
Then in Eclipse: File | Import... | General | Existing Projects into Workspace

### Vagrant build environment

```bash
vagrant up
vagrant ssh
cd /vagrant/
```

## Running

### Interactive

In interactive mode, `qore-llvm` expects one statement per line.

```bash
qore-llvm
```

### Compile to LLVM IR

```bash
qore-llvm -l [-o output] source.q
```

### Compile to a native executable

Running

```bash
ninja sandbox
```
or 
```bash
make sandbox
```
compiles `tools/sandbox/test.q` into a native executable located in `bin/sandbox`.
