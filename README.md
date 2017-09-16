# [Mustard](https://en.wikipedia.org/wiki/List_of_Cluedo_characters#Colonel_Mustard) kernel

A _very_ basic kernel written in NASM and C, with probably some C++ and other higher level languages in the future. At the moment it's extememely lacking in features, but I hope to implement most if not all of these:

 - A file system
 - A shell, similar to bash but not
 - Some kind of package manager to install programs from a repository.
 - A graphical desktop (very ambitious)
 - Probably even more stuff
 
I can see a file system and a shell being implemented fairly soon, but the other features probably won't be implemented for a while.

# Building

Installing dependencies

```
sudo apt-get install qemu nasm grub
```

Navigate to the root of the project (the directory with the Makefile in it)

```
make clean
make all
```

# Emulating

Assuming you've already built the kernel, and it's in `dist/`

```
make run
```

But if you've moved it somewhere for whatever reason, it'll still work fine to do this

```
qemu-system-i386 -kernel /path/to/kernel
```
