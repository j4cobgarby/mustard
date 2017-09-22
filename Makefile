ASM=nasm
ASMFLAGS=-felf32
CC=gcc
CCFLAGS=-m32 -c -fno-stack-protector
LD=ld
LDFLAGS=-melf_i386

all:
	mkdir -p obj
	mkdir -p dist
	$(ASM) $(ASMFLAGS) kernel.asm -o obj/kasm.o
	$(CC) $(CCFLAGS) kernel.c -o obj/kc.o
	$(LD) $(LDFLAGS) -T link.ld -o dist/kernel obj/kasm.o obj/kc.o

iso:
	cp dist/kernel isodir/boot/mustard.bin
	grub-mkrescue -o mustard.iso isodir

run:
	qemu-system-i386 -kernel dist/kernel

clean:
	@rm -f obj/*
	@rm -f dist/*
	@rm -f mustard.iso
	@rm -f isodir/boot/mustard.bin
	@rm -rf obj
	@rm -rf dist
