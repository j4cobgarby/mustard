bits 32		; nasm directive, to say it should generate code for a 32 bit processor
section .text

; grub multiboot
	align	4
	dd	0x1BADB002				; magic number to identify the header
	dd	0x00					; flags
	dd - (0x1BADB002 + 0x00)	; checksum
; end of multiboot part

global start
extern boot						; defined in c file

start:
	cli							; block interrupts so the cpu can't be woken from the hlt instruction
	mov		esp, stack_space	; should already be set but just to make sure
	call	boot				; call function from c file
	hlt							; halts the cpu

section .bss
	stack_space: resb 8192