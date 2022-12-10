# kvfs - A simple key-value store filesystem for Linux.

## Usage Example

Once the filesystem is mounted, you can use it as following. We'll assume it's mounted at /dev/kvs1 for this example. You may mount as many kvfs instances as you need.

```bash
# Create a new empty key called 'my_key'
$ echo -n my_key > /dev/kvs1/_mk

# Set the value of 'my_key' to 'Hello, world!'
$ echo -n "Hello, world!" > /dev/kvs1/my_key

# Read the value of 'my_key'
$ cat /dev/kvs1/my_key

# Delete the key 'my_key'
$ echo -n my_key > /dev/kvs1/_del
```

The above example accesses the key-value store using shell commands, which can be a convenient way to do it. Of course, though, this filesystem can be used just as well from any programming language.

```python
mkf = open("/dev/kvs1/_mk", "wb", buffering=0)
delf = open("/dev/kvs1/_del", "wb", buffering=0)

# Create a new key. You may need to mkf.flush().
mkf.write(b"newkey")

k1 = open("/dev/kvs1/newkey", "r+b", buffering=0)
k1.write(b"Some data")
print(k1.read())

delf.write(b"newkey")

mkf.close()
delf.close()
k1.close()
```

Hopefully it's easy to see that this functionality could be nicely wrapped into a library, that would make it very simple for a programmer to use this key value store.

## Installation

Build the kernel module with either `make`, or running `init.sh`.

`make` will just build the kernel module (which will be in `build/kvfsmod.ko`, whereas running `init.sh` will both build it _and_ mount an instance of the filesystem to `/dev/kvs1`.

To build, you will need to have:

 - A C compiler toolchain (we use gcc)
 - The linux headers/module build scripts (which on Arch linux are in the package `linux-headers`)
