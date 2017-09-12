ASM=nasm
ASMFLAGS=-felf32
CC=gcc
CCFLAGS=-m32 -c
LD=ld
LDFLAGS=-melf_i386

all:
	mkdir -p obj
	mkdir -p dist
	$(ASM) $(ASMFLAGS) kernel.asm -o obj/kasm.o
	$(CC) $(CCFLAGS) kernel.c -o obj/kc.o
	$(LD) $(LDFLAGS) -T link.ld -o dist/kernel obj/kasm.o obj/kc.o

run:
	qemu-system-i386 -kernel dist/kernel

clean:
	rm -f obj/*
	rm -f dist/*
	rm -rf obj
	rm -rf dist