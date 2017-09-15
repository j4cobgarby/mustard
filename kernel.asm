bits 32
section .data
        ; multiboot header for grub
        align 4
        dd 0x1BADB002              	; magic num
        dd 0x00                    	; flags
        dd -(0x1BADB002 + 0x00)		; checksum, should = 0

section .text
global start
global keyboard_handler
global read_port
global write_port
global load_idt

; functions written in c
extern boot
extern keyboard_handler_main

read_port:
	mov		edx, [esp + 4]
	in		al, dx
	ret

write_port:
	mov		edx, [esp + 4]    
	mov		al, [esp + 4 + 4]  
	out		dx, al  
	ret

load_idt:
	mov		edx, [esp + 4]
	lidt	[edx]
	sti 				; turn on interrupts
	ret

keyboard_handler:                 
	call	keyboard_handler_main
	iretd

start:
	cli 				; block cpu interrupts
	mov		esp, stack_space
	call	boot
	hlt 				; halt cpu

section .bss
stack_space: resb 8192	; 8KB stack
