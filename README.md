# qore-llvm

## Building

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
