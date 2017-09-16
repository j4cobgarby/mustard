#include "keyboard_map.h"
#include "io.h"
#include "shell.h"

void boot(void) {
    // splash
    char *splash = "--- Mustard kernel ---";
    clear_screen();
	printa(splash, 0x1e0);
	nl();
	np();

    // initialize IDT for responses to interrupts and exceptions
    idt_init();
    // initialize driver for keyboard
    keyboard_init();
    // draw cursor
	update_cursor_graphic();

    // event loop
	for(;;);
}
