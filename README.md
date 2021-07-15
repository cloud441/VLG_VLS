# VLG_VLS
Repository of Very Large Graph EPITA course project to implement a over-simplified graph spanner.

## How to use

### Build the project

```bash
mkdir build
cd build
cmake ..
```

### Understand the binary

```bash
./vls -h
```

### Use the binary

This is just a way of use the binary. Please look at the previous help to get what you want in option.
Please download the inet graph from Clemence Magnien to try our code.

```bash
./vls -f ../data/inet --bfs-strategy community --bfs-number 15
```
