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
